#include "stdafx.h"
#include "Brown.h"
#include <AceBuilding.h>

CBrown::CBrown(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CBrown::CBrown(const CBrown& rhs)
    : Base(rhs)
{
}

CBrown::~CBrown()
{
}

CBrown* CBrown::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Monster Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z); // ������ġ ����
    pInstance->m_pTextureComp->Set_Scale({3.f, 3.f, 1.f});
    pInstance->m_pTextureComp->Readjust_Transform();
    pInstance->m_tStat.vPatrolPointZero = { _x, _y, _z }; // ������ġ�� ��������Ʈ ���� 
    return pInstance;
}

CBrown* CBrown::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("Monster Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CBrown::Ready_GameObject()
{
    SUPER::Ready_GameObject();

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    //���� ����
    m_tSound.m_fTalkAge = 0.f;
    m_tSound.m_fTalkLife = 5.f; // �ݺ��� �ʿ��Ѿֵ��� ���� �̰ű������� 
    m_tSound.m_fTalkReapeat = 0.f;
    m_tSound.m_fSoundVolume = 0.6f;
    m_tSound.m_fSoundEffectVolume = 0.2f;
    m_tSound.m_bSoundOnce = FALSE;
    m_tSound.m_bSoundCheck = FALSE;

    //�̹��� �� ������ ���� 
    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
    m_pTextureComp->Readjust_Transform();
    
    m_tFrame.fFrame = 0.f;
    m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    m_tFrame.fFrameSpeed = 12.f;
    m_tFrame.fRepeat = 0.f;

    // ��������Ʈ ���� 
    Set_TeamID(ETEAM_MONSTER);

    // Status
    m_tStat.fAttackDistance = 13.f;
    m_tStat.fAttackFov = 90.f;
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;
    m_bDazedState = FALSE;
    m_bDeadState = FALSE;
    m_bDazeToHeal = FALSE;

    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
    m_bCollisionOn = FALSE;
    //m_pColliderComp->Set_Scale({ 0.2, 0.2, 0.2 });
   // m_pColliderComp->Set_ScaleY(0.2);

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    // ���
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBrown::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBrown::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CBrown::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CBrown::AI_Rest);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBrown::AI_Chase);

    // �߰�
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBrown::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBrown::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::STRAFING, &CBrown::AI_Strafing);
    m_tState_Obj.Add_Func(STATE_OBJ::INCHFORWARD, &CBrown::AI_InchForward);
    //m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CBrown::AI_Jump);
    
    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::NORMALATTACK, &CBrown::AI_NormalATTACK);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);

    // �ǰ�
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CBrown::AI_Hit);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CBrown::AI_Falling);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CBrown::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CBrown::AI_CrotchHit);

    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CBrown::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CBrown::AI_Death);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CBrown::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CBrown::AI_Headless);

    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CBrown::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBrown::AI_GoHome);


#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
    // STATE�� ::A �϶� CBrownŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBrown::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CBrown::Approach);
    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CBrown::Moving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CBrown::Attack);
    m_tState_Act.Add_Func(STATE_ACT::FALLING, &CBrown::Falling);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBrown::GoHome);
    
#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
    m_mapActionKey.Add_Action(ACTION_KEY::INCHFORWARD); //����ﶧ
    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING); // ����ﶧ
    m_mapActionKey.Add_Action(ACTION_KEY::JUMP); // y�࿡ ���̰� ������ 
    m_mapActionKey.Add_Action(ACTION_KEY::NORMALATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);
    m_mapActionKey.Add_Action(ACTION_KEY::BEPUSHED); // ��ħ�������� 

#pragma endregion

    return S_OK;
}

HRESULT CBrown::Ready_GameObject(const FSerialize_GameObject tObjectSerial)
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

    m_ftheta = 0.f; 

    return S_OK;
}

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Gravity(fTimeDelta);

    m_pTransformComp->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

    // ����Ÿ�� 
    if (m_pTransformComp->Get_Pos().y < 1.6f && m_vSpeed.y < 0.f)
    {
        Height_On_Terrain(1.6f);
        m_IsOnGround = true;
    }
    else
        m_IsOnGround = false;

    //�״°� ���� - ���� �������� �߻��ذ� 
    if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
        MonsterDead();

    if (FALSE == m_bDeadState)
        Billboard(fTimeDelta);

    //���¸ӽ�
    m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
    {
        m_tFrame.fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State() ||
            STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
            m_tFrame.fRepeat += 1;
    }

    // ������
    Update_InternalData();
    
    //���� ������Ʈ �ڵ�
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region �⺻ override 

void CBrown::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBrown::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));
    m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
    m_pBufferComp->Render_Buffer();

#pragma region �浹 �޽� �ݶ��̴�
    //MeshSphereColider(_float(pSphereShape->fRadius), 32, 16);
#pragma endregion

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBrown::Add_Component()
{
    // �浹 ������Ʈ 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderCapsuleComp"), E_FAIL);
    
    // ���� ���� ���
    m_pColliderComp->EnterToPhysics(0);
    
    // �浹 �Լ� ����
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);
    
    // �浹 ���̾�, ����ũ ����
    m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER); // �� Ŭ������ ���� �浹���̾� 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_PROJECTILE | LAYER_WALL | LAYER_PLAYER_ATTACK | LAYER_BOSS_SKILL); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

    m_pColliderComp->Set_Scale(_vec3(0.75f, 2.8f, 0.75f));

    return S_OK;
}

void CBrown::Free()
{
    SUPER::Free();
}

#pragma endregion 

#pragma region �浹 - OnCollision 

void CBrown::OnCollision(CGameObject* pDst, const FContact* const pContact) // ��� �浹�� 
{
    CSolid* pSolid = dynamic_cast<CSolid*>(pDst);
    if (pSolid)
    {
        _vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
        m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
        if (D3DXVec3Dot(&(-vNormal), &_vec3({ 0.f, -1.f, 0.f })) < 0.f)
            m_IsOnGround = true;
    }
}

void CBrown::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) // ó�� �浿 ���� 
{
    if (Get_IsMonsterDeath())
        return;
    else if (25 >= m_gHp.Cur && FALSE == m_bDazedState)
        m_tState_Obj.Set_State(STATE_OBJ::DAZED);

    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;

    // ���� ��ų�� �������� �ֱ⶧���� �浹ü ���� �˻��ѵڿ� dead�� �Ѿ����
    // ���� �� : ����  ������� : �÷��̾� 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // �������
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

        if (nullptr == pPlayer)
        {
            CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

            if (nullptr == pPlayerAttack)
                return;
            else
            {
                //==== �÷��̾� ����ü��  �浹 =============================
                m_bCollisionOn = true;
                m_eRecentCol = RECENT_COL::PLAYERATK;

                Add_BasicEffect(m_pOwner); // ����Ʈ �߰�

                if (m_tStat.iDazedHP >= m_gHp.Cur && FALSE == m_bDazedState)
                {
                    OutputDebugString(L"��Brown - �浹���� DAZED ����   \n");
                    m_tState_Obj.Set_State(STATE_OBJ::DAZED);
                    return;
                }
                else if (STATE_RIGHTHAND::KICK == ePlayerRighthand)
                    m_tState_Obj.Set_State(STATE_OBJ::FALLING); // fist
                else if (PSITDONW_ATTACK == m_ePlayer_AttackState) // �� + kick = falling �� 
                    m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
                else //�Ͼ�� FacePunch���� 
                {
                    if (Random_variable(60))
                        m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
                    else
                        m_tState_Obj.Set_State(STATE_OBJ::HIT);
                }
            }
        }
        else
        {
            //==== �÷��̾�� �浹 =====================================
            m_eRecentCol = RECENT_COL::PLAYER;

            switch (ePlayerRighthand)
            {
            case Engine::STATE_RIGHTHAND::RUN_HAND:
                m_bCollisionOn = true;
                Add_BasicEffect(m_pOwner); // ����Ʈ �߰�
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); //�޸��� 
                break;
            case Engine::STATE_RIGHTHAND::KICK:
                m_bCollisionOn = true;
                Add_BasicEffect(m_pOwner); // ����Ʈ �߰�
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); //�޸��� 
                break;
            default:
                m_bCollisionOn = true;
                break;
            }
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::FRIEND)
    {
        // ģ�а��� - ���� : ���ʹ� ģ�������� ������ ������ 
        // ���� ��ü�� �浹�� ���� ������ ��ȯ�� ����ü���� �浹�� 

        CMonsterAttackUnion* pBossAttack = dynamic_cast<CMonsterAttackUnion*>(pAceObj);
       
        if(nullptr == pBossAttack)
            return;
        else
        {
            // ==== ������ų�� �浹 ========================================
            m_bCollisionOn = true;
            m_eRecentCol = RECENT_COL::BOSSATK;

            Add_BasicEffect(m_pOwner); // ����Ʈ �߰�

            m_tState_Obj.Set_State(STATE_OBJ::HIT);
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::NUETRAL) // ������Ʈ �浹 
    {
        m_tState_Obj.Set_State(STATE_OBJ::HIT);
    }

    // �ǰ��� 40���� Ȯ���� �ݰ� ���� 
    if (Random_variable(40))
    {
        if (Random_variable(50))
            m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);
        else
            m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
    }
}

void CBrown::OnCollisionExited(CGameObject* pDst) // �浹 ������ 
{
    m_bCollisionOn = false;
}

void CBrown::MonsterDead()
{
    if(RECENT_COL::PLAYER == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    else if (RECENT_COL::PLAYERATK == m_eRecentCol)
    {
        if (STATE_RIGHTHAND::GUN == ePlayerRighthand ||
            STATE_RIGHTHAND::THOMPSON == ePlayerRighthand)
            m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        else
            m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    }
    else if(RECENT_COL::BOSSATK == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    else // ���Ͱ� �ν��������ϴ� ���� �߰� 
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
}
#pragma endregion 

#pragma region BlackBoard

void CBrown::Update_InternalData()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Monster.Get_BlackBoard())
    {
        m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
        // ���� ����
        if (!m_wpBlackBoard_Monster.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

    if (m_bCollisionOn)
    {
        pBlackBoard->Get_MonsterHP() = m_gHp;
        pBlackBoard->Get_Owner() = L"Unarmed Goon";

        Engine::Add_GameObject(L"UI", L"UI_MonsterHpBar", CUI_MonsterHP::Create(m_pGraphicDev));
    }
}

void CBrown::Update_BlackBoard()
{
    //// ������ ���� ���, ���� �ڵ�� �ʼ�
	//if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//{
	//	m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
	//	// ���� ����
	//	if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//		return;
	//}

	//// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	//CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

	//// ���⼭���� �������� ������ ���´�.
	//m_fHp = pBlackBoard->Get_HP().Cur;
}

#pragma endregion 

#pragma region ȯ�漳�� �μ���Ʈ + ���¸ӽ� �����Լ� 

#pragma endregion 

#pragma region ���¸ӽ� : idle ~ Death

//------------------ AI ---------------------------
void CBrown::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : idle ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player())
        {
            //Engine::Play_Sound(L"Enemy", L"GOON_MothNFlame_Fire.wav", SOUND_ENEMY, VoiceDistance());
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        m_bArrive = FALSE;
          //OutputDebugString(L"��Brown - ���¸ӽ� : idle �� \n");
    }
}

void CBrown::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Suspicious ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Suspicious");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

       Engine::Add_GameObject(L"GameLogic", CEffect_Awareness::Create(m_pGraphicDev,
           m_pTransformComp->Get_Pos().x + 0.1f, 
           m_pTransformComp->Get_Pos().y + 1.3f,
           m_pTransformComp->Get_Pos().z, CEffect_Awareness::TYPE::BROWN, this));

       Engine::Play_Sound(L"Enemy", L"Brown_Suspicious.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        
        if(Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            _float fWeight = 4.0f / Calc_Distance(); 

            m_tStat.fAwareness += fDeltaTime * 4.f * (1 + fWeight); // ����ġ �߰��� 

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // �߰����� ���� 
            }
        }
        else // �������� ����
        {
            ////OutputDebugString(L"��Brown - ����üũ : �������� ������   \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    };

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Suspicious ��   \n");
    }
}

void CBrown::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Taunt ����   \n");
         m_tFrame.fFrameSpeed = 7.f;
         m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Taunt");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

         Engine::Play_Sound(L"Enemy", L"Brown_Taunt.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (2 == m_tFrame.fRepeat) // ���� �ι� �ϰ� ���󰡱� 
        {
            m_tFrame.fRepeat = 0; //�ٸ����� ���ϱ� 0���� �ǵ����� 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
        }

    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Taunt ��   \n");
    }
}

void CBrown::AI_Chase(float fDeltaTime) // �޸��ٰ� �ȴٰ� �쳯�������� 
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chease ����   \n");
       // m_pTransformComp->m_vScale.x = 0.5f;
        m_tFrame.fFrameSpeed = 10.f; //���󺹱� 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_tFrame.fFrame = 0.f;

        if(Random_variable(10))
            Engine::Play_Sound(L"Enemy", L"Brown_RndTaunt.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
        else if (Random_variable(10))
            Engine::Play_Sound(L"Enemy", L"Brown_RndTaunt.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
    }
    if (m_tState_Obj.Can_Update())
    {
        m_AttackOnce = false;
        _float CurDistance = Calc_Distance();

        //OutputDebugString(L"�� Brown ����� : Chease - Update ����   \n");

        if (Detect_Player()) // ��Ÿ��� �����Ǵ°�� 
        {
            //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� \n");
            // --------�Ÿ��� ���¸ӽ� -----------
            // �پ �ٰ��� : a > 8
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - Run ����  \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - Walk ���� \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // ���� : 4 < a <= 7
            else if (m_fInchDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - Moving ���� \n");
                int iCombo = (rand() % 10) + 1;

                if (5 <= iCombo) // 60% / 5~10 
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (5 > iCombo) // 40% / 1~4 
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFING);
            }
            // ������
            else
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - ���� ����  \n");
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // ��Ÿ��� �÷��̾ ��������� 
        {
            //OutputDebugString(L"��Brown - ���¸ӽ� : �÷��̾� ��ħ    \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f; // ������ ���� 

            if (0 >= m_tStat.fAwareness) //�������� �ʱ�ȭ
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ��ħ - Recon���� �Ѿ  \n");
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE); 
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chease ��   \n");
    }
}

void CBrown::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Rest ����   \n");

        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 15.f;
        m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Rest ��   \n");

    }
}

void CBrown::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Run ����   \n");

         m_tFrame.fFrameSpeed = 11.f;
         m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"RunSouth");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

         Engine::Play_Sound(L"Enemy", L"Brown_Run.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (Calc_Distance() < m_fWalkDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Run ��   \n");
    }
}

void CBrown::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Walk ����   \n");
        m_tFrame.fFrameSpeed = 14.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrame = 0.f;

        // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
        //Engine::Play_Sound(L"Enemy", L"Brown_Walk.wav", SOUND_ENEMY_MONSTER, m_tSound.m_fSoundVolume);

    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (Calc_Distance() < m_fWalkDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Walk ��   \n");

    }
}

void CBrown::AI_InchForward(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ����   \n");
        m_tFrame.fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"InchForward");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

        //if (Random_variable(50))
        //    Engine::Play_Sound(L"Enemy", L"Brown_InchA.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
        //else
        //    Engine::Play_Sound(L"Enemy", L"Brown_InchB.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
        {
            //OutputDebugString(L"�� ����� ã�� : INCHFORWARD - ����Ű ������ ����   \n");
            m_mapActionKey[ACTION_KEY::INCHFORWARD].Act();
        }
        if (Calc_Distance() <= m_fInchDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            //OutputDebugString(L"�� ����� ã�� : INCHFORWARD - ������ �� ���� \n");
            //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ��   \n");
            m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ��   \n");

    }
}

void CBrown::AI_Strafing(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Strafing ����   \n");

         m_tFrame.fFrameSpeed = 9.f;
         m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Strafing");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;
         m_tFrame.fRepeat = 0.f;

         if(Random_variable(50))
             Engine::Play_Sound(L"Enemy", L"Brown_StrafingA.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
         else
             Engine::Play_Sound(L"Enemy", L"Brown_StrafingB.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }
    if (m_tState_Obj.Can_Update())
    {     
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::STRAFING].Act();

        if (m_tFrame.fRepeat >=3)
        {
            m_tFrame.fRepeat = 0.f;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Strafing ��   \n");
    }
}

void CBrown::AI_NormalATTACK(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Attack ����   \n");
        m_tFrame.fFrameSpeed = 8.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"NormalAttack");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

         Engine::Play_Sound(L"Enemy", L"Brown_Punch.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tFrame.fFrame > 3.f && m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::NORMALATTACK].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Attack ��   \n");
    }
}

void CBrown::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : HeavyAttack ����   \n");

        m_tFrame.fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"HeavyAttack");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

         if(Random_variable(50))
             Engine::Play_Sound(L"Enemy", L"Brown_HeavyAttackA.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
         else
             Engine::Play_Sound(L"Enemy", L"Brown_HeavyAttackB.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > 4.f && m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CBrown::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Hit ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Hit");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 10.f;
         m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame >=  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST); 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Hit ��   \n");
    }
}

void CBrown::AI_Falling(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Falling ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Falling");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
        m_tFrame.fFrame = 0.f;
        m_bSecondFall = true;
        m_AttackOnce = true;

        Engine::Play_Sound(L"Enemy", L"Brown_Falling.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BEPUSHED].Act();

        if (m_tFrame.fFrame > 4.f && m_AttackOnce)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_FallingDust::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y - 1.f, m_pTransformComp->Get_Pos().z, this));
            m_AttackOnce = false;
            m_bSecondFall = false;
        }

        if (m_tFrame.fFrame > 9.f && !m_AttackOnce && !m_bSecondFall)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_FallingDust::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y - 1.f, m_pTransformComp->Get_Pos().z, this));
            m_bSecondFall = true;
        }

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Falling ��   \n");
    }
}

void CBrown::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"FacePunch");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 8.f;
         m_tFrame.fFrame = 0.f;

         if(Random_variable(50))
             Engine::Play_Sound(L"Enemy", L"Brown_FacePunchA.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
         else
             Engine::Play_Sound(L"Enemy", L"Brown_FacePunchB.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ��   \n");
    }
}

void CBrown::AI_CrotchHit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"CrotchHit");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 2.f; // 2���� CHASE ���� 
        m_tFrame.fFrame = 0.f;

        Engine::Play_Sound(L"Enemy", L"Brown_CrotchHit.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);
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
        //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ��   \n");
    }
}

void CBrown::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Dazed");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 1.f;
        m_tFrame.fFrame = 0.f;
        m_bDazedState = TRUE;
        m_AttackOnce = false;

        Engine::Play_Sound(L"Enemy", L"Brown_Dazed.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (!m_AttackOnce)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_DazeSwirl::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z, this));
            m_AttackOnce = true;
        }

        m_tFrame.fAge += 1.f * fDeltaTime;

        if (m_tFrame.fAge > m_tFrame.fLifeTime)
        {
            m_tFrame.fLifeTime = 0.f;
            m_tFrame.fAge = 0.f;
            m_bDazeToHeal = TRUE;
        }
        if (TRUE == m_bDazeToHeal)
        {
            if (m_gHp.Update(fDeltaTime * 10.f, 40.f, TRUE)) // ������, �����ϸ� bool��ȯ 
            {
                m_gHp.Cur = 40.f;
                m_bDazedState = FALSE;
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ��   \n");
    }
}

void CBrown::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Chopped ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Chopped");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;
        Engine::Play_Sound(L"Enemy", L"Brown_Choped.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chopped ��   \n");
    }
}

void CBrown::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Headless ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Headless");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 11.f;
         m_tFrame.fFrame = 0.f;
         m_bDeadState = TRUE;

         Engine::Play_Sound(L"Enemy", L"Brown_Headless.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd-1)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Headless ��   \n");
    }
}

void CBrown::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Death ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Death");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;

        Engine::Play_Sound(L"Enemy", L"Brown_Death.wav", SOUND_ENEMY_BROWN, m_tSound.m_fSoundVolume);

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame> m_tFrame.fFrameEnd-1)
        {
            m_tFrame.fFrame =  m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Death ��   \n");
    }
}

void CBrown::AI_Reconnaissance(float fDeltaTime)
{
    //�÷��̾� ���ļ� �������� 0���� ����������� 
    if (m_tState_Obj.IsState_Entered())
    {
        m_tFrame.fFrame = 0.f;
          //OutputDebugString(L"��Brown - ���¸ӽ� : Reconnaissance ����   \n");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //�÷��̾� �����ϸ� �ٽ� ���� 
        {
            m_tStat.fAwareness += fDeltaTime * 6.f; // �������� �� ������ �����Ұ� 
            if (m_tStat.fAwareness >= m_tStat.fMaxAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT);
            }
        }
        else // �÷��̾� ����x
            m_tStat.fConsider -= fDeltaTime * 3.f;

        if (0.f >= m_tStat.fConsider) 
        {
            // ��躯������ ������ 0�� �Ǹ� ������ ���� 
            m_tStat.fConsider = 10.f; // �ٽ� �ʱ� �������� 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME); 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Reconnaissance ��   \n");
    }
}

void CBrown::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : GoHome ����   \n");
           m_tFrame.fFrameSpeed = 15.f;
           m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_North");
           m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
           m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // ���� �׼�Ű�� IDLE �̹Ƿ� 
            m_mapActionKey[ACTION_KEY::GOHOME].Act(); // �׼�Ű ������ 

        if (Detect_Player())
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        
        //Calc_Theta();

       // switch (m_eDirection)
       // {
       // case CAceMonster::Dir::EAST:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_West");
       //     m_pTextureComp->Set_RotationY( - D3DX_PI);
       //     break;
       // case CAceMonster::Dir::SOUTHEAST:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_SouthWest");
       //     m_pTextureComp->Set_RotationY(-D3DX_PI);
       //     break;
       // case CAceMonster::Dir::SOUTH:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
       //     break;
       // case CAceMonster::Dir::SOUTHWEST:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_SouthWest");
       //     break;
       // case CAceMonster::Dir::WEST:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_West");
       //     break;
       // case CAceMonster::Dir::NORTHWEST:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_NorthWest");
       //     break;
       // case CAceMonster::Dir::NORTH:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_North");
       //     break;
       // case CAceMonster::Dir::NORTHEAST:
       //     m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_NorthWest");
       //     m_pTextureComp->Set_RotationY(-D3DX_PI);
       //     break;
       // }

        if (m_bArrive && (m_tFrame.fFrame >  m_tFrame.fFrameEnd)) // ������ �� ���� 
        {
            m_bArrive = FALSE;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : GoHome ��   \n");
    }
}

#pragma endregion 

#pragma region �ൿ�ӽ� Idle ~ GoHome

void CBrown::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        if (m_mapActionKey[ACTION_KEY::INCHFORWARD].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::MOVING);
        if (m_mapActionKey[ACTION_KEY::STRAFING].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::MOVING);

        if (m_mapActionKey[ACTION_KEY::NORMALATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);
        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);

        if (m_mapActionKey[ACTION_KEY::BEPUSHED].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::FALLING);

    }

    if (m_tState_Act.IsState_Exit()) // ���� �ʿ��Ҷ��� �־ - ã�ƺ��� 
    {
    }
}

void CBrown::Approach(float fDeltaTime) // RUN �׼�Ű ���� �����ϴ°�
{
    if (m_tState_Act.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - �ൿ�ӽ� : Approach ����   \n");
    }

    // ����
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon���� - �켱���� 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);

        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
              //OutputDebugString(L"��Brown - ����Ű : RUN ����   \n");
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 8.f);
        }

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
        { 
            //OutputDebugString(L"��Brown - ����Ű : WALK ����   \n");
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 6.f);
    
        }


        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
          ////OutputDebugString(L"��Brown - �ൿ�ӽ� : Approach ��   \n");
    }
}
void CBrown::Moving(float fDeltaTime)
{

    if (m_tState_Act.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ����   \n");
    }
    // ����
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon����
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        if(Calc_Distance() < m_tStat.fAttackDistance && Calc_Distance() > m_fRunDistance ) // Run���̷� ��ġ�� Run���� 
            m_tState_Obj.Set_State(STATE_OBJ::RUN);

        if (STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : INCHFORWARD ����   \n");

            // ���Ͱ� �÷��̾ ���º��� = �� - �� 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos(); // �÷��̾� ������ �̵� 
            D3DXVec3Normalize(&vDir, &vDir);

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 8.f);

            if (Calc_Distance() < m_fInchDistance) //inch ��Ÿ����� ������ �ٸ��� �����ϱ����ؼ� 
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            
        }

        if (STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : STRAFING ����   \n");

            _vec3 MoveDir = m_pTransformComp->Get_Right() + m_pTransformComp->Get_Look(); // �÷��̾� ������ 
            D3DXVec3Normalize(&MoveDir, &MoveDir);
            m_pTransformComp->Move_Pos(&MoveDir, fDeltaTime, 7.f);

            if (Calc_Distance() < m_fInchDistance) //inch ��Ÿ����� ������ �ٸ��� �����ϱ����ؼ� 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);

        }
        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ��   \n");
    }
}

void CBrown::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - �ൿ�ӽ� : ATTACK ����   \n");
    }

    // ����
    {
        //���Ͱ� �ٶ󺸴������� ���� 
        _vec3 vLook = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vLook, &vLook);

        _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

        if (STATE_OBJ::NORMALATTACK == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : BASICATTACK ����   \n");
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
            //OutputDebugString(L"��Brown - ����Ű : HEAVYATTACK ����   \n");
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
        // //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ��   \n");
    }
}

void CBrown::Falling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - �ൿ�ӽ� :FALLING ����   \n");
    }

    // ����
    {
        vDir = m_pPlayerTransformcomp->Get_Look();

        D3DXVec3Normalize(&vDir, &vDir);

        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 7.f);

        if (STATE_OBJ::FALLING != m_tState_Obj.Get_State())
            m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"��Brown - �ൿ�ӽ� : GOHOME ��   \n");
    }
}


void CBrown::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - �ൿ�ӽ� : GoHome ����   \n");
    }

    // ����
    {
        vDirect = m_tStat.vPatrolPointZero - m_pTransformComp->Get_Pos();

        _float fDistance = D3DXVec3Length(&vDirect);

        D3DXVec3Normalize(&vDirect, &vDirect);

        if (2.f >= fDistance)
        {
            m_bArrive = true;
            m_tState_Act.Set_State(STATE_ACT::IDLE);
        }
        else
        {
            //OutputDebugString(L"��Brown - ���� ��Ʈ�� ����Ʈ ������   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 6.f);
        }
    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"��Brown - �ൿ�ӽ� : GOHOME ��   \n");
    }
}

#pragma endregion