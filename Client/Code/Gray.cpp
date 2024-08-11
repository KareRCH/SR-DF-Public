#include "stdafx.h"
#include "Gray.h"
#include <AceBuilding.h>

CGray::CGray(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CGray::CGray(const CGray& rhs)
	: Base(rhs)
{
}

CGray::~CGray()
{
}

CGray* CGray::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("GrayMonster Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
    pInstance->m_pTextureComp->Set_Scale({ 3.5f, 6.f, 1.f });
    pInstance->m_pTextureComp->Readjust_Transform();
    pInstance->m_tStat.vPatrolPointZero = { _x, _y, _z };
    return pInstance;
}

CGray* CGray::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("GrayMonster Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CGray::Ready_GameObject()
{
    SUPER::Ready_GameObject();

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    //사운드 관련
    m_tSound.m_fTalkAge = 0.f;
    m_tSound.m_fTalkLife = 5.f; // 반복이 필요한애들은 대충 이거기준으로 
    m_tSound.m_fTalkReapeat = 0.f;
    m_tSound.m_fSoundVolume = 0.6f;
    m_tSound.m_fSoundEffectVolume = 0.2f;
    m_tSound.m_bSoundOnce = FALSE;
    m_tSound.m_bSoundCheck = FALSE;

    // 이미지 관련
    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
    m_pTextureComp->Readjust_Transform();
    //m_pTextureComp->Set_Scale({ 3.f, 5.f, 1.f });

    m_tFrame.fFrame = 0.f;
    m_tFrame.fFrameEnd= _float(m_pTextureComp->Get_VecTexture()->size());
    m_tFrame.fFrameSpeed= 12.f;
    m_tFrame.fRepeat= 0.f;
    m_tFrame.fAge = 0.f;

    // 팀에이전트 셋팅 
    Set_TeamID(ETEAM_MONSTER);

    //Status
    m_tStat.fAttackDistance = 13.f;
    m_tStat.fAttackFov = 80.f;
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;
    m_bDazedState = FALSE;
    m_bDeadState = FALSE;
    m_bDazeToHeal = FALSE;

    // 충돌용
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
    //m_pColliderComp->Set_Scale({ 0.1, 0.1, 0.1 });

    m_bCollisionOn = FALSE;

#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // 초기상태 지정 
    // 추격
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CGray::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CGray::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::YOUDIE, &CGray::AI_YouDie);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CGray::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CGray::AI_Chase); 
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CGray::AI_Rest);
    
    // 기본견제
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CGray::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CGray::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEPEYE, &CGray::AI_KeepEye);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDEWALK, &CGray::AI_SideWalk);
    m_tState_Obj.Add_Func(STATE_OBJ::FRIGHTEN, &CGray::AI_Frighten);
    m_tState_Obj.Add_Func(STATE_OBJ::UPRIGHTRUN, &CGray::AI_UpRightRun);
    
    // 공격
    m_tState_Obj.Add_Func(STATE_OBJ::THROW, &CGray::AI_Throw);
    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CGray::AI_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CGray::AI_HeavyAttack);
    
    // 복귀
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CGray::AI_GoHome);
    
    // 막기
    m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    
    // 피격
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CGray::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CGray::AI_Falling);
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CGray::AI_Hit);
    
    // 사망
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);

#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CGray::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CGray::Approach);
    m_tState_Act.Add_Func(STATE_ACT::SUDDENATTACK, &CGray::SuddenAttack);
    m_tState_Act.Add_Func(STATE_ACT::SIDEMOVING, &CGray::SideMoving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CGray::Attack);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CGray::GoHome);
    m_tState_Act.Add_Func(STATE_ACT::FALLING, &CGray::Falling);

#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); //가까울때

    m_mapActionKey.Add_Action(ACTION_KEY::KEEPEYE); // 가까울때
    m_mapActionKey.Add_Action(ACTION_KEY::SIDEWALK); // y축에 차이가 있을때 

    m_mapActionKey.Add_Action(ACTION_KEY::UPRIGHT); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::FRIGHTEN); //중간

    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::BACK);

    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);

#pragma endregion

    return S_OK;
}

HRESULT CGray::Ready_GameObject(const FSerialize_GameObject tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTextureComp->Set_Scale(tObjectSerial.vScale);
    m_pTextureComp->Readjust_Transform();
    //m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    m_tStat.vPatrolPointZero = m_pTransformComp->Get_Pos();

    m_pColliderComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());

    return S_OK;
}

_int CGray::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Gravity(fTimeDelta);

    m_pTransformComp->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

    // 지형타기 
    if (m_pTransformComp->Get_Pos().y < 1.5f && m_vSpeed.y < 0.f)
    {
        Height_On_Terrain(1.5f);
        m_IsOnGround = true;
    }
    else
        m_IsOnGround = false;


    // 몬스터 죽이기 
    if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
        MonsterDead();

    // 빌보드 
    if (FALSE == m_bDeadState)
        Billboard(fTimeDelta);


    // 상태머신
    m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
    {
        m_tFrame.fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State() ||
            STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State() ||
            STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State()
            )
            m_tFrame.fRepeat += 1;
    }

#pragma region 실험실

#pragma endregion 

    //블랙보드 업로드 
    Update_InternalData();

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region 기본 환경설정 

void CGray::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CGray::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));
    m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
    m_pBufferComp->Render_Buffer();

#pragma region 충돌 메쉬 콜라이더
    //MeshSphereColider(_float(pSphereShape->fRadius), 32, 16);
 #pragma endregion

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CGray::Add_Component()
{
    // 충돌 컴포넌트 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderCapsuleComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_WALL | LAYER_PLAYER_ATTACK | LAYER_BOSS_SKILL); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

    m_pColliderComp->Set_Scale(_vec3(0.75f, 2.8f, 0.75f));

    return S_OK;
}

void CGray::Free()
{
    SUPER::Free();
}


#pragma endregion 

#pragma region 환경설정 부속파트 + 상태머신 보조함수 

void CGray::RenderSplitImages()
{
    _vec3 MonsterLook = m_pTransformComp->Get_Look();
    _vec3 PlayerLook = m_pPlayerTransformcomp->Get_Look();

    _float DotProduct = D3DXVec3Dot(&MonsterLook, &PlayerLook);

    //if(DotProduct)

    swprintf_s(debugString, L"Brown - 변수 확인 DotProduct = %f\n", DotProduct);
    //OutputDebugStringW(debugString);
}

#pragma endregion

#pragma region 충돌파트 

void CGray::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
    //OutputDebugString(L"▶Gray 충돌중 \n");

    CAceBuilding* pSolid = dynamic_cast<CAceBuilding*>(pDst);
    if (pSolid)
    {
        _vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
        m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
        if (D3DXVec3Dot(&(-vNormal), &_vec3({ 0.f, -1.f, 0.f })) < 0.f)
            m_IsOnGround = true;
    }
}

void CGray::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    // 충돌 대상 : 플레이어, 플레이어 공격체, 보스몬스터 공격체, 중립(플레이어가 던지는 아이템) 
    if (Get_IsMonsterDeath())
        return;
    else if (25 >= m_gHp.Cur && FALSE == m_bDazedState)
        m_tState_Obj.Set_State(STATE_OBJ::DAZED);

    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;

    // 보스 스킬에 죽을수도 있기때문에 충돌체 먼저 검사한뒤에 dead로 넘어가야함
    // 현재 팀 : 몬스터  적대관계 : 플레이어 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // 적대관계
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

        if (nullptr == pPlayer)
        {
            CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

            if (nullptr == pPlayerAttack)
                return;
            else
            {
                //==== 플레이어 공격체와  충돌 =============================
                m_bCollisionOn = true;
                m_eRecentCol = RECENT_COL::PLAYERATK;

                Add_BasicEffect(m_pOwner); // 이펙트 추가

                if (m_tStat.iDazedHP >= m_gHp.Cur && FALSE == m_bDazedState)
                {
                    OutputDebugString(L"▷Brown - 충돌판정 DAZED 진입   \n");
                    m_tState_Obj.Set_State(STATE_OBJ::DAZED);
                    return;
                }
                else if (STATE_RIGHTHAND::KICK == ePlayerRighthand)
                    m_tState_Obj.Set_State(STATE_OBJ::FALLING); // fist
                else if (PSITDONW_ATTACK == m_ePlayer_AttackState) // 앉 + kick = falling 임 
                    m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
                else //일어서야 FacePunch있음 
                {
                    if (Random_variable(50))
                        m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
                    else
                        m_tState_Obj.Set_State(STATE_OBJ::HIT);
                }
            }
        }
        else
        {
            //==== 플레이어와 충돌 =====================================
            m_eRecentCol = RECENT_COL::PLAYER;
            switch (ePlayerRighthand)
            {
            case Engine::STATE_RIGHTHAND::RUN_HAND:
                m_bCollisionOn = true;
                Add_BasicEffect(m_pOwner); // 이펙트 추가
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); //달릴때 
                break;
            case Engine::STATE_RIGHTHAND::KICK:
                m_bCollisionOn = true;
                Add_BasicEffect(m_pOwner); // 이펙트 추가
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); //달릴때 
                break;
            default:
                break;
            }
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::FRIEND)
    {
        // 친밀관계 - 보스 : 몬스터는 친밀이지만 보스는 적대임 
        // 보스 자체의 충돌은 없고 보스가 소환한 공격체에는 충돌함 

        CMonsterAttackUnion* pBossAttack = dynamic_cast<CMonsterAttackUnion*>(pAceObj);

        if (nullptr == pBossAttack)
            return;
        else
        {
            // ==== 보스스킬과 충돌 ========================================
            m_bCollisionOn = true;
            m_eRecentCol = RECENT_COL::BOSSATK;

            Add_BasicEffect(m_pOwner); // 이펙트 추가
            m_tState_Obj.Set_State(STATE_OBJ::HIT);
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::NUETRAL) // 오브젝트 충돌 
    {
        m_tState_Obj.Set_State(STATE_OBJ::HIT);
        Add_BasicEffect(m_pOwner); // 이펙트 추가
    }

    // 피격의 40프로 확률로 반격 시전 - 반격의 종류는 반반확률  
    if (Random_variable(40))
    {
        if (Random_variable(50))
            m_tState_Obj.Set_State(STATE_OBJ::THROW);
        else
            m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
    }
}

void CGray::OnCollisionExited(CGameObject* pDst)
{
    m_bCollisionOn = FALSE;
}

#pragma region 블랙보드 

void CGray::Update_InternalData()
{
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Monster.Get_BlackBoard())
    {
        m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
        // 연결 실패
        if (!m_wpBlackBoard_Monster.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

    if (m_bCollisionOn)
    {
        pBlackBoard->Get_MonsterHP() = m_gHp;
        pBlackBoard->Get_Owner() =  L"Pipe Wielding Mook";

        Engine::Add_GameObject(L"UI", L"UI_MonsterHpBar", CUI_MonsterHP::Create(m_pGraphicDev));

        /*_float cur = pBlackBoard->Get_BossHP().Cur;
        swprintf_s(debugString, L"블랙보드 변수 확인 curBoss = %f\n", cur);
        OutputDebugStringW(debugString);


        _float cur2 = pBlackBoard->Get_BrownHP().Cur;
        swprintf_s(debugString, L"블랙보드 변수 확인 curBrown  = %f\n", cur2);
        OutputDebugStringW(debugString);


        _float cur3 = pBlackBoard->Get_GrayHP().Cur;
        swprintf_s(debugString, L"블랙보드 변수 확인 curGray = %f\n", cur3);
        OutputDebugStringW(debugString);*/
    }
}

void CGray::Update_BlackBoard()
{  
    //// 블랙보드 연결 대기, 안전 코드로 필수
    //if (!m_wpBlackBoard_Player.Get_BlackBoard())
    //{
    //	m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
    //	// 연결 실패
    //	if (!m_wpBlackBoard_Player.Get_BlackBoard())
    //		return;
    //}

    //// 안전 코드를 거치면 일반 포인터로 접근 허용.
    //CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    //// 여기서부터 블랙보드의 정보를 얻어온다.
    //m_fHp = pBlackBoard->Get_HP().Cur;
}

#pragma endregion
void CGray::MonsterDead()
{
    if (RECENT_COL::PLAYER == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    else if (RECENT_COL::PLAYERATK == m_eRecentCol)
    {
        if (STATE_RIGHTHAND::GUN == ePlayerRighthand)
            m_tState_Obj.Set_State(STATE_OBJ::HEADSHOT);
        else if(STATE_RIGHTHAND::THOMPSON == ePlayerRighthand)
            m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        else
            m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    }
    else if (RECENT_COL::BOSSATK == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    else
    m_tState_Obj.Set_State(STATE_OBJ::DEATH);
}
#pragma endregion 

#pragma region 목표(AI) 상태머신 부분 
void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : idle 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : idle 끝  \n");
    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Suspicious 진입  \n");
       Engine::Add_GameObject(L"GameLogic", CEffect_Awareness::Create(m_pGraphicDev,
           m_pTransformComp->Get_Pos().x ,
           m_pTransformComp->Get_Pos().y + 1.4f,
           m_pTransformComp->Get_Pos().z, CEffect_Awareness::TYPE::GRAY, this));
           m_tFrame.fFrame = 0.f;

       Engine::Play_Sound(L"Enemy", L"Gray_Suspicious.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume); 

    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            _float fWeight = 4.0f / Calc_Distance();

            m_tStat.fAwareness += fDeltaTime * 5.f * (1 + fWeight); // 가중치 추가본 

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = m_tStat.fAwareness; // 추후 감소를 위해 최대값으로 고정 

               // int iCombo = (rand() % 10) + 1; // 1~10 
               //
               // if (6 <= iCombo) // 6~10
                    m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
               //
               // if (6 > iCombo) // 1~5
               //     m_tState_Obj.Set_State(STATE_OBJ::TAUNT);

            }
        }
        else // 범위밖은 감소
        {
            //OutputDebugString(L"▶Gray - 변수체크 : 인지변수 감소중   \n");

            m_tStat.fAwareness -= fDeltaTime * 6.f;

            //플레이어가 시야각을 벗어나 인지값이 초기화되면 idle로 back
            if (0 >= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Suspicious 끝  \n");

    }
}

void CGray::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Taunt 진입  \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Taunt");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
        m_tFrame.fFrame = 0.f;
        Engine::Play_Sound(L"Enemy", L"Gray_Taunt.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume); 
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            if (2 == m_tFrame.fRepeat) // 도발 두번 하고 따라가기 
            {
                m_tFrame.fRepeat = 0; //다른데도 쓰니까 0으로 되돌리기 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Taunt 끝  \n");
    }
}

void CGray::AI_YouDie(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : You Die 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 9.f;
        m_tFrame.fFrame = 0.f;
        Engine::Play_Sound(L"Enemy", L"Gray_YouDie.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd) // 도발 두번 하고 따라가기 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : You Die 끝   \n");
    }
}

void CGray::AI_Reconnaissance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_tFrame.fFrame = 0.f;

    }
    if (m_tState_Obj.Can_Update())
    {
        m_tStat.fConsider -= fDeltaTime * 3.f;

        if (Detect_Player())
        {
            m_tStat.fAwareness += fDeltaTime * 5.f; // 이전보다 더 빠르게 증가할것 

            if (m_tStat.fAwareness >= m_tStat.fMaxAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
            }
        }

        if (0 >= m_tStat.fConsider)
        {
            m_tStat.fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"WalkNorth");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 14.f;
        m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        if (Detect_Player())
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        if (m_bArrive && m_tFrame.fFrame > m_tFrame.fFrameEnd) // 프레임 다 돌면 
        {
            m_bArrive = FALSE;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CGray::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Chase 진입  \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 8.f;
        m_tFrame.fFrame = 0.f;

        m_AttackOnce = FALSE;
    }

    if (m_tState_Obj.Can_Update())
    {
        //OutputDebugString(L"★ Gray 디버깅 : Chease - Update 돌입   \n");

        if (Detect_Player()) // 플레이어 포착 
        {
            _float CurDistance = Calc_Distance();

            if (m_bPlayerAttakBool && m_bCollisionOn == FALSE) // 플레이어는 공격상태 + 충돌은 안한상태 
                m_tState_Obj.Set_State(STATE_OBJ::BLOCK);

            // 뛰어서 다가옴 : 8 < a <= 13
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease -RUN 상태함수 Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // 걸어서 다가옴 : 7.5 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - WALK 상태함수 Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // 주시하면서 경계 : 6 < a <= 7.5
            else if (m_fEyesOnYouDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - 경계 상태함수 Set    \n");
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            //대충 공격하러 옴 : 3 < a <=6
            else if (m_fCloseToYouDistance < CurDistance && m_fEyesOnYouDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - 접근 상태함수 Set  \n");
                int iCombo = (rand() % 15) + 1;

                if (4 > iCombo)                         // 20프로 1~3
                    m_tState_Obj.Set_State(STATE_OBJ::THROW);
                else if ( 10 > iCombo && 4 <= iCombo)   // 40프로  4 ~ 9
                    m_tState_Obj.Set_State(STATE_OBJ::UPRIGHTRUN);
                else                                    // 40프로 10~15
                    m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
            }
            else  // 공격함
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - 공격 상태함수 Set   \n");
                int iCombo = (rand() % 10) + 1;

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                else
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }

        }
        else // 쫒다가도 시야에서 벗어나면 게이지 줄어들어서 SUSPICIOUS로 돌아감 
        {
            //OutputDebugString(L"★ Gray 디버깅 : Chease - 포착실패   \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_tStat.fAwareness) //인지값이 초기화되면 
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
            }

        }
    }
     
    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Chase 끝   \n");
    }
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //일종의 숨고르기 구간임 
        //OutputDebugString(L"▷Gray - 상태머신 : Rest 돌입   \n");

        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Rest 끝   \n");
    }
}

void CGray::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Run 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Run");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 17.f;
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"Gray_Run.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 RUN 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        // 조건 - 플레이어가 시야각으로 들어오면 
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Run 끝   \n");
    }
}

void CGray::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Walk 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Walk");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 11.f;
        m_tFrame.fFrame = 0.f;
        Engine::Play_Sound(L"Enemy", L"Gray_Walk.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Walk 끝   \n");
    }
}

void CGray::AI_KeepEye(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : KeepEye 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"KeepEye");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 11.f;
        m_tFrame.fFrame = 0.f;
        Engine::Play_Sound(L"Enemy", L"Gray_Keepeye.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPEYE].Act();

        // 조건 - 플레이어가 시야각으로 들어오면 
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : KeepEye 끝 \n");
    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : SideWalk 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"SideWalk");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f; 
        Engine::Play_Sound(L"Enemy", L"Gray_SideWalk.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume); 
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SIDEWALK].Act();

        if (2 == m_tFrame.fRepeat)
        {
            if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
            {
                m_tFrame.fRepeat = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : SideWalk 끝 \n");
    }
}

void CGray::AI_Throw(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Throw 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Throw");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"Gray_ThrowPipe.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
        Engine::Play_Sound(L"Enemy", L"_ThrowPipe.wav", SOUND_ENEMY_NORMAL_EFFECT, m_tSound.m_fSoundVolume);


        // 투사체 발사 
        Engine::Add_GameObject(L"GameLogic", CThrowPipe::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 1.f, m_pTransformComp->Get_Pos().z, this, (ETEAM_ID)Get_TeamID()));
    
    }

    if (m_tState_Obj.Can_Update())
    {
        // 충돌체는 여기서 가상키 누르고 state에서 할예정 

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Throw 끝   \n");
    }
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : UpRightRun 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"UpRightRun");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 13.f;
        m_tFrame.fFrame = 0.f; 
        Engine::Play_Sound(L"Enemy", L"Gray_Upright.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume); 

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::UPRIGHT].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : UpRightRun 끝   \n");
    }
}

void CGray::AI_Frighten(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Frighten 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Frighten");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"Gray_Frighten.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::FRIGHTEN].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Frighten 끝   \n");
    }
    
}

void CGray::AI_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Attack 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Attack");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"_PipeSwing.wav", SOUND_ENEMY_NORMAL_EFFECT, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > 4.f && m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BASIC_ATTACK].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Attack 끝   \n");
    }
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeavyAttack");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"_PipeHeavySwing.mp3", SOUND_ENEMY_NORMAL_EFFECT, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > 5.f && m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CGray::AI_Block(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Block 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Block");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        Engine::Play_Sound(L"Enemy", L"Gray_Block.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume); 

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CGray::AI_CrotchHit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : CrotchHit 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"CrotchHit");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 2.f; // 2초후 CHASE 진입 
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"Gray_CrotchHit.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        m_tFrame.fAge += 1.f * fDeltaTime;

        if (m_tFrame.fAge > m_tFrame.fLifeTime)
        {
            m_tFrame.fLifeTime = 0.f;
            m_tFrame.fAge = 0.f;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : CrotchHit 끝   \n");
    }
}

void CGray::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : FacePunch 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"FacePunch");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;

        if(Random_variable(50))
            Engine::Play_Sound(L"Enemy", L"Gray_FacePunchA.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
        else
            Engine::Play_Sound(L"Enemy", L"Gray_FacePunchB.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : FacePunch 끝   \n");
    }
}

void CGray::AI_Falling(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 진입   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Falling");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        m_AttackOnce = true;
        m_bSecondFall = true;

        Engine::Play_Sound(L"Enemy", L"Gray_Falling.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BACK].Act();

        if (m_tFrame.fFrame > 4.f && m_AttackOnce)
        {
           Engine::Add_GameObject(L"GameLogic", CEffect_FallingDust::Create(m_pGraphicDev,
                                 m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z, this));
           m_AttackOnce = false;
           m_bSecondFall = false;
        }

        if (m_tFrame.fFrame > 8.f && !m_AttackOnce && !m_bSecondFall)
        {
           Engine::Add_GameObject(L"GameLogic", CEffect_FallingDust::Create(m_pGraphicDev,
                                  m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z, this));
           m_bSecondFall = true;
        }

        
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 끝   \n");
    }
}

void CGray::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 진입   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Coward");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;

        if (Random_variable(50))
            Engine::Play_Sound(L"Enemy", L"Gray_HitA.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
        else
            Engine::Play_Sound(L"Enemy", L"Gray_HitB.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 끝   \n");
    }
}

void CGray::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Dazed   \n");
        // 일어나는것까지 진행함 
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Dazed");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 1.f;
        m_tFrame.fFrame = 0.f;
        m_bDazedState = TRUE; 
        
        Engine::Play_Sound(L"Enemy", L"Gray_Dazed.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        m_tFrame.fAge += 1.f * fDeltaTime;

        if (m_tFrame.fAge > m_tFrame.fLifeTime)
        {
            m_tFrame.fLifeTime = 0.f;
            m_tFrame.fAge = 0.f;
            m_bDazeToHeal = TRUE;
        }
        if (TRUE == m_bDazeToHeal)
        {
            if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
                m_tFrame.fFrame = 10.f;

            if (m_gHp.Update(fDeltaTime * 3.f, 40.f, TRUE)) // 증가값, 도달하면 bool반환 
            {
                m_gHp.Cur = 40.f;
                m_bDazedState = FALSE;
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Dazed 끝   \n");
    }
}

void CGray::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Chopped   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Chopped");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;

        Engine::Play_Sound(L"Enemy", L"Gray_Chopped.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
            // 이상태로 잠자는사운드 추가 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Chopped 끝   \n");
    }
}

void CGray::AI_HeadShot(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  HeadShot   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeadShot");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 7.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;

        Engine::Play_Sound(L"Enemy", L"Gray_Headshot.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeadShot 끝   \n");
    }
}

void CGray::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Headless   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Headless");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 7.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;

        Engine::Play_Sound(L"Enemy", L"Gray_Headshot.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Headless 끝   \n");
    }
}

void CGray::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Death   \n");
        // 일어나는것까지 진행함 
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Death");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_tFrame.fFrameSpeed = 7.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;

        Engine::Play_Sound(L"Enemy", L"Gray_Death.wav", SOUND_ENEMY_GRAY, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Death 끝   \n");
    }
}


#pragma endregion

#pragma region 행동 상태머신 부분 
void CGray::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Idle 진입   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        // 일반 다가오는파트 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        // 주시하며 경계
        if (m_mapActionKey[ACTION_KEY::KEEPEYE].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        if (m_mapActionKey[ACTION_KEY::SIDEWALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        // 
        if (m_mapActionKey[ACTION_KEY::UPRIGHT].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        if (m_mapActionKey[ACTION_KEY::FRIGHTEN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        // 공격파트 
        if (m_mapActionKey[ACTION_KEY::BASIC_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        // 피격 
        if (m_mapActionKey[ACTION_KEY::BACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::FALLING);
        
        // 패트롤파트 
        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);

    }

    if (m_tState_Act.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Idle 끝   \n");
    }
}

void CGray::Approach(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Approach 진입   \n");
    }

    // 실행
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로 - 우선사항 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        //몬스터가 플레이어 바라보는 벡터 
        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);
        
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 9.f);
        
        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 6.5f);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Approach 끝   \n");
    }
}

void CGray::SideMoving(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Side - KeepEYE  OR  SideWalk 진입   \n");
    }

    // 실행
    {   
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);
        
        if (STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, 3.f);
        }

        if (STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, 4.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Side - KeepEYE  OR  SideWalk 끝   \n");
    }
}

void CGray::SuddenAttack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Sudden - Upright  OR  Frighten 진입   \n");
    }

    // 실행
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        else
        {
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
            D3DXVec3Normalize(&vDir, &vDir);

            if (STATE_OBJ::UPRIGHTRUN == m_tState_Obj.Get_State())
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 5.f);

            if (STATE_OBJ::FRIGHTEN == m_tState_Obj.Get_State())
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 6.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Sudden - Upright  OR  Frighten 끝   \n");
    }
}

void CGray::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Attack 진입   \n");
    }

    // 실행
    {
        _vec3 vLook = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vLook, &vLook);

        _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

        if (STATE_OBJ::ATTACK == m_tState_Obj.Get_State())
        {
            if (!m_AttackOnce)
            {

                Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
                    vDirPos.x, vDirPos.y, vDirPos.z,
                    CMonsterPunch::TYPE::NORMAL, this, (ETEAM_ID)Get_TeamID()));

                m_AttackOnce = true;
            }
        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
            if (!m_AttackOnce)
            {
               
                Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
                    vDirPos.x, vDirPos.y, vDirPos.z,
                    CMonsterPunch::TYPE::HEAVY, this, (ETEAM_ID)Get_TeamID()));

                m_AttackOnce = true;
            }
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Attack 끝   \n");
    }
}

void CGray::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
    {
        _vec3 vDirect = m_tStat.vPatrolPointZero - m_pTransformComp->Get_Pos();

        _float fDistance = D3DXVec3Length(&vDirect);

        D3DXVec3Normalize(&vDirect, &vDirect);

        if (2.f >= fDistance)
        {
            m_bArrive = true;
            m_tState_Act.Set_State(STATE_ACT::IDLE);
        }
        else
        {
            ////OutputDebugString(L"▷Brown - 기존 패트롤 포인트 복귀중   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 3.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::Falling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
    {
        vDir = m_pPlayerTransformcomp->Get_Look();

        D3DXVec3Normalize(&vDir, &vDir);

        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 7.f);

        if (STATE_OBJ::FALLING != m_tState_Obj.Get_State())
            m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}


#pragma endregion