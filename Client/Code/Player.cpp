#include "stdafx.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Solid.h"
#include "CalculatorComponent.h"
#include "ColliderComponent.h"
#include "PlayerLighter.h"
#include "MonsterAttackUnion.h"
#include "PlayerGunLighter.h"
#include "Management.h"
#include "BlackBoard_Player.h"
#include "AceBuilding.h"
#include "DynamicCamera.h"
#include "Effect_HitPow.h"
#include "AceUnit.h"
#include "AceWeapon.h"
#include "PlayerLightning.h"
#include "Effect_CartridgeCase.h"

//테스트
#include "UI_PlayerHurt.h"
#include "UI_EatFood.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
    Set_ObjectName(L"Player");
}

CPlayer::CPlayer(const CPlayer& rhs)
    : Base(rhs)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

#pragma region 팀 아이디

    Set_TeamID(ETEAM_PLAYER);

#pragma endregion
    

#pragma region 블랙보드

    Engine::Add_BlackBoard(L"Player", CBlackBoard_Player::Create());
    Engine::Add_BlackBoard(L"MonsterUnion", CBlackBoard_Monster::Create());

#pragma endregion

#pragma region 플레이어 충돌체
    //m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    //FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
    //pShape->fRadius = 0.5f;

    /*FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->fRadius = 5.f;*/
#pragma endregion

    //사운드 관련
    m_tPlayerSound.m_fTalkAge = 0.f;
    m_tPlayerSound.m_fTalkLife = 5.f; // 반복이 필요한애들은 대충 이거기준으로 
    m_tPlayerSound.m_fTalkReapeat = 0.f;
    m_tPlayerSound.m_fSoundVolume = 0.6f;
    m_tPlayerSound.m_fSoundEffectVolume = 0.2f;
    m_tPlayerSound.m_bSoundOnce = false;
    m_tPlayerSound.m_bSoundCheck = false;

#pragma region 플레이어 크기 및 위치 설정 (초기 값)
    // 플레이어 정보 (초기값)
    m_gHp.Max = 100.f;
    m_gHp.Cur = m_gHp.Max;
    m_fChage.Max = 2.f; // 차지를 완료 할 시간
    m_tTime.fChargeStartTime = 1.f; // 차지를 시작할 시간
    m_bGunLight = false;
    bRightPunch = true;
    bLeftPunch = false;
    m_PlayerLighter = false;

    m_tLeftHand.bPickUpState = true;
    m_tRightHand.bPickUpState = true;

    //m_pMeshComp->BoxMesh_Ready(m_pGraphicDev, 10.f, 10.f, 5.f);

    // 플레이어 행렬 초기화
    m_pTransformComp->Set_Pos({ 10.f, 10.f, 10.f }); // Test Stage
    //m_pTransformComp->Set_Pos({ 20.f, 5.f, -9.5f }); // Jump Map
    m_pTransformComp->Readjust_Transform();

    m_pLeftHandComp->Set_Pos({ -300.f, -363.f, 0.f });	                        // 이미지 위치
    m_pLeftHandComp->Set_Scale({ 1495.f, 1588.f, 1.f });						// 이미지 크기

    m_pRightHandComp->Set_Pos({ 0.f, 0.f, 0.f });	                            // 이미지 위치
    m_pRightHandComp->Set_Scale({ 1495.f, 1588.f, 1.f });						// 이미지 크기
#pragma endregion

#pragma region 초기 상태 세팅 (현재 상태)  
    m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);           // 플레이어 
    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);       // 왼손  
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);;    // 오른손
#pragma endregion


#pragma region 플레이어의 상태 추가
    m_tPlayer_State.Add_Func(STATE_PLAYER::IDLE, &ThisClass::Idle);         // 기본 상태
    m_tPlayer_State.Add_Func(STATE_PLAYER::SITDOWN, &ThisClass::Down);      // 앉은 상태
    m_tPlayer_State.Add_Func(STATE_PLAYER::DIE, &ThisClass::Die);           // 죽은 상태
#pragma endregion

#pragma region 플레이어의 행동 추가
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::IDLE, &ThisClass::Action_Idle);              // 기본 (정지)
    //m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::MOVE, &ThisClass::Action_Move);              // 이동
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::RUN, &ThisClass::Action_Run);                       // 달리기
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::CHARGING, &ThisClass::Action_Charging);      // 차징
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::THROW_AWAY, &ThisClass::Action_ThrowAway);   // 버리기
#pragma endregion

#pragma region 플레이어의 왼손 상태 추가
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::NONE, &ThisClass::Left_None);                // 왼손 없음
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::HAND, &ThisClass::Left_Hand);                // 왼손 주먹
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_RIHGTER, &ThisClass::Left_RunRighter);   // 왼손 라이터 들고 뛰고있음
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::OPEN_HAND, &ThisClass::Left_OpenHand);       // 오른손 오브젝트
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_HAND, &ThisClass::Left_RunHand);         // 뛰고있음
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RIGHTER, &ThisClass::Left_Righter);          // 라이터
#pragma endregion


#pragma region 플레이어의 오른손 상태 추가
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::NONE, &ThisClass::Right_None);             // 오른손 없음
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::HAND, &ThisClass::Right_Hand);             // 오른손 주먹
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::GUN, &ThisClass::Right_Gun);               // 권총
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::RUN_HAND, &ThisClass::Right_RunHand);      // 달리기
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::THOMPSON, &ThisClass::Right_Thompson);     // 톰슨 기관총
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::STEELPIPE, &ThisClass::Right_Steelpipe);   // 쇠파이프
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::BEERBOTLE, &ThisClass::Right_BeerBotle);   // 맥주병
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::FRYINGPAN, &ThisClass::Right_FryingPan);   // 프라이팬
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::KICK, &ThisClass::Right_Kick);             // 발차기
#pragma endregion

#pragma region 액션 키
    for (size_t i = 0; i < static_cast<size_t>(EACTION_KEY::SIZE); i++)
        m_tActionKey.Add_Action(static_cast<EACTION_KEY>(i));
#pragma endregion

    // Tset (오브젝트 받아오는거)
    m_eObjectType = OBJECT_TYPE::NONE; // 초기상태 : 양손 주먹
    m_eObjectName = OBJECT_NAME::NONE; // 초기상태 : 없음

    return S_OK;
}

HRESULT CPlayer::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
    //m_pTexture->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    // 플레이어 행렬 초기화
    m_pTransformComp->Readjust_Transform();

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    if (m_bGunLight)
        m_fAge += 1.f * fTimeDelta;

    if (m_fAge >= m_fLifeTime)
    {
        m_fAge = 0.f;
        m_bGunLight = false;
    }

    // 마우스 움직임
    Mouse_Move();
    // 마우스 입력
    Attack_Input(fTimeDelta);
    // 키보드 입력
    Keyboard_Input(fTimeDelta);

    // 상태 업데이트(체크)
    State_Update(fTimeDelta);

    PlayerJump(fTimeDelta);

    // 지형 타기
    if (m_pTransformComp->Get_Pos().y < 1.7f && m_vSpeed.y < 0.f)
    {
        Height_On_Terrain();
        m_IsOnGround = true;
    }
    else
        m_IsOnGround = false;

    // 대쉬
    Dash(fTimeDelta);

    // 프레임 관리
    FrameManage(fTimeDelta);

    // 물리 업데이트 코드
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌체 이동

    //플레이어 디버그용 코드 
    //swprintf_s(debugString, L"Player - 변수 확인 m_gHp = %f\n", m_gHp.Cur);
    //OutputDebugStringW(debugString);


    //플레이어 뒤로 콜라가 생성되는 코드
    if (Engine::IsKey_Pressed(DIK_K))
    {
        Engine::Add_GameObject(L"UI", CUI_PlayerHurt::Create(m_pGraphicDev));
    }
    if (Engine::IsKey_Pressed(DIK_L))
    {
        Engine::Add_GameObject(L"UI",CUI_EatFood::Create(m_pGraphicDev));
    }
        //Engine::Add_GameObject(L"GameLogic", CAceObjectFactory::Create(m_pGraphicDev,CAceObjectFactory::OBJECT_CLASS::FOOD, 
    //    L"Test", m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z));

    //_vec3 vTest = m_pTransformComp->Get_Pos();
    //vTest.z += 1.f;
    //list<CGameObject*> listCollision = Engine::IntersectTests_Sphere_GetGameObject(0, vTest, 3.f);
    //for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
    //{
    //    if ((*iter) != this) // 자기자신을 죽이지 않는 보안코드 삽입 
    //        (*iter)->Set_Dead();
    //}
        // 현재 피격 테스트중 


    m_tActionKey.Update();

    // 블랙보드 연동하기
    Update_BlackBoard();

    // 블랙보드 내려받기
    Update_InternalData();

    // 랜더 그룹 지정, 현재상태 : 알파 테스트
    Engine::Add_RenderGroup(RENDER_UI, this);

    // 랜더 그룹 목록
    //RENDER_PRIORITY, RENDER_ALPHATEST, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END

    return 0;
}

void CPlayer::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
#pragma region 옵션
    // 행렬 적용
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    //m_pMeshComp->BoxMesh_Col();
    // 랜더 상태 옵션
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#pragma endregion

#pragma region 왼손
    // 왼손 위치 설정
    m_pLeftHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pLeftHandComp->Get_Transform());

    // 왼손 출력 여부
    if (m_tLeftHand.bLeftHandOn)
    {
        // 왼손 텍스처 출력
        m_pLeftHandComp->Render_Texture((_ulong)m_tLeftHand.fLeftFrame, true);    // 왼손 텍스처 출력
        m_pBufferComp->Render_Buffer();                                 // 왼손 버퍼 
        int t = 0;
    }
#pragma endregion

#pragma region
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    if (bDbugFrame)
    {
       // m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
       //
       // MeshSphereColider(10.f, 30, 30);
       // MeshBoxColider(5.f, 20.f, 20.f);
       // //m_pBufferComp->Render_Buffer();
       //
       // m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#pragma endregion

#pragma region 오른손
    // 오른손 위치 설정
    m_pRightHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pRightHandComp->Get_Transform());

    // 오른손 출력 여부
    if (bRightHandOn)
    {
        m_pRightHandComp->Render_Texture(_uint(m_tRightHand.fRightFrame), true);    // 오른손 텍스처 출력
        m_pBufferComp->Render_Buffer();                             // 오른손 버퍼
    }
#pragma endregion

#pragma region 옵션 끄기 
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#pragma endregion
}

HRESULT CPlayer::Add_Component()
{
#pragma region 컴포넌트
    // 버퍼 컴포넌트
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    // 행렬 컴포넌트
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    // 지형타기 컴포넌트
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    // 콜라이더 컴포넌트
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderCapsuleComp"), E_FAIL);
    // 왼손 컴포넌트
    NULL_CHECK_RETURN(m_pLeftHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftHandTest", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // 오른손 컴포넌트
    NULL_CHECK_RETURN(m_pRightHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightHandTest", L"Proto_PlayerRightTextureComp"), E_FAIL);
#pragma endregion
    //NULL_CHECK_RETURN(m_pMeshComp = Set_DefaultComponent_FromProto<CMeshColComp>(ID_STATIC, L"Com_Mesh", L"Proto_MeshColBufferComp"), E_FAIL);

    // 텍스처 매니저
    // 왼손
    m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
    // 오른손
    m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);
    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);
    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER);
    m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER |
                                      LAYER_BOSS_SKILL | LAYER_MONSTER_ATTACK |
                                      LAYER_ITEM | LAYER_WALL);
    m_pColliderComp->Set_Scale(_vec3(1.f, 3.f, 1.f));
    m_pColliderComp->Readjust_Transform();
    // 플레이어가 shift로 대쉬를 하거나 공격을 했을때만 몬스터와 충돌이 허용됨. 

    return S_OK;
}

bool CPlayer::Keyboard_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);

    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = m_tCamera.vAt;

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

#pragma region 키입력
    // 전진
    if (Engine::IsKey_Pressing(DIK_W))
    {
        // 플레이어 행동 : 걷기
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);
        
        if (!m_tRightHand.bPickUpState)
        {
            // 뛰기
            if (Engine::IsKey_Pressing(DIK_LSHIFT))
            {
                // 전진 속도 Up
                m_tPlayer.fStraightSpeed = 10.f;
                // 플레이어 행동 : 달리기
                m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::RUN);
                m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
                m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);

                m_tRightHand.bRightFrameOn = true;
                m_tLeftHand.bLeftFrameOn = true;
            }
        }
        // 뛰기
        if (Engine::IsKey_Released(DIK_LSHIFT))
        {
            // 플레이어 모두 : 초기화
            m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::NONE);
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);

            // 전진 속도 복구
            m_tPlayer.fStraightSpeed = 5.f;
            m_tRightHand.bRightFrameOn = false;
            m_tRightHand.fRightFrame = 0.f;
            m_tLeftHand.bLeftFrameOn = false;
            m_tLeftHand.fLeftFrame = 0.f;
            m_tTime.fLeftCurrentTime = 0.f;
            m_tTime.fRightCurrentTime = 0.f;
        }

        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, m_tPlayer.fStraightSpeed);
    }



    // 후진
    if (Engine::IsKey_Pressing(DIK_S))
    {
        // 플레이어 행동 : 걷기
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);

        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -m_tPlayer.fSpeed);

        // S + Shift 뒷 대쉬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_tDash.fDownDash = 0.f;    // 대쉬 높이(카메라) 설정
            m_tDash.fDash = 20.f;       // 대쉬 값 설정
            bDashOn = true;     // 대쉬 On/Off
            m_eDashDir = DOWN;  // 대쉬 방향
        }
    }

    // 오른쪽
    if (Engine::IsKey_Pressing(DIK_D))
    {
        // 플레이어 행동 : 걷기
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);

        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, m_tPlayer.fSpeed);

        // fSpeed = 5.f , fDash = 20.f
        // D + Shift 우측 대쉬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_tDash.fDownDash = 0.f;    // 대쉬 높이(카메라) 설정
            m_tDash.fDash = 20.f;       // 대쉬 값 설정
            bDashOn = true;     // 대쉬 On/Off
            m_eDashDir = RIGHT; // 대쉬 방향
        }
    }

    // 왼쪽
    if (Engine::IsKey_Pressing(DIK_A))
    {
        // 플레이어 행동 : 걷기
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);

        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, m_tPlayer.fSpeed);

        // A + Shift 좌측 대쉬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_tDash.fDownDash = 0.f;    // 대쉬 높이(카메라) 설정
            m_tDash.fDash = 20.f;       // 대쉬 값 설정
            bDashOn = true;     // 대쉬 On/Off
            m_eDashDir = LEFT;  // 대쉬 방향
        }
    }

    // 손
    if (Engine::IsKey_Pressed(DIK_1))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_HAND;
        m_eObjectName = OBJECT_NAME::NONE;
    }

    // 권총
    if (Engine::IsKey_Pressed(DIK_2))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::GUN;
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
    }

    // 톰슨 기관총
    if (Engine::IsKey_Pressed(DIK_3))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_OBJECT;
        m_eObjectName = OBJECT_NAME::THOMPSON;
    }

    // 쇠파이프
    if (Engine::IsKey_Pressed(DIK_4))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::STEELPIPE;
    }

    // 맥주병
    if (Engine::IsKey_Pressed(DIK_5))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::BEERBOTLE;
    }

    // 프라이팬
    if (Engine::IsKey_Pressed(DIK_6))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::FRYINGPAN;
    }

    // 점프
    if (Engine::IsKey_Pressed(DIK_SPACE))// && m_IsOnGround)
    {
        m_vSpeed.y = 11.f;
    }

    // 발차기
    //if (Engine::IsKey_Pressed(DIK_Q))
    //{
    //    m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK);
    //}

    // 라이터
    if (Engine::IsKey_Pressed(DIK_V) && m_tPlayer_Action.Get_State() != STATE_PLAYER_ACTION::RUN)
    {
        m_tLeftHand.bLeftFrameOn = true;

        if (!bRighter)  // 라이터가 꺼져있을 경우
        {
            bRighter = true;        // 라이터 켜주기
            Engine::Play_Sound(L"FallenAces", L"Open_Zippo.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER); // 왼손 상태 라이터로
            m_tLeftHand.fLeftFrame = 0.f;
            m_tTime.fLeftCurrentTime = 0.f;
        }
        else // 라이터가 켜져있을 경우
        {
            Engine::Play_Sound(L"FallenAces", L"Close_Zippo.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            bBackRighter = true; // 라이터 되돌리기On
            m_tLeftHand.fLeftFrame = 0.f;
            m_tTime.fLeftCurrentTime = 0.f;
        }
    }

    // 오브젝트 버리기 (오브젝트가 있을 경우만 실행가능)
    if (Engine::IsKey_Pressed(DIK_F) && m_eObjectName != OBJECT_NAME::NONE)
    {
        // 플레이어 상태 오브젝트 버리는 중
        Engine::Play_Sound(L"FallenAces", L"Drop_Item.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::THROW_AWAY);
    }

    // 앉기 (누르는 중)
    if (Engine::IsKey_Pressing(DIK_C))
    {
        if (m_tPlayer_State.Get_State() != STATE_PLAYER::SITDOWN) // 이미 앉은상태가 아닐때 
        {        
            // 플레이어 상태 앉기
            m_tPlayer_State.Set_State(STATE_PLAYER::SITDOWN);

            _vec3 vSitDown = { 0.f, 5.f, 0.f };

            // 대쉬 높이
            m_pTransformComp->Set_MovePos(fTimeDelta, vSitDown);
            //m_pTransformComp->Set_PosY(-10.f);
        }
        else //앉은상태일때 -> 일어서야함 
            m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
    }

    // Test
    if (Engine::IsKey_Pressed(DIK_E))
    {
        LineEvent();
    }


    // 뗐을 때
    if (Engine::IsKey_Released(DIK_C))
    {
        // 플레이어 상태 되돌림
        m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
    }

    // 공격 프레임 확인
    if (Engine::IsKey_Pressed(DIK_0))
    {
        if (bDbugFrame)
        {
            bDbugFrame = false;
        }
        else
        {
            bDbugFrame = true;
        }
        
    }
    
    // 오브젝트 상호작용 // 소영 추가 
    /*if (Engine::IsKey_Pressed(DIK_E) && Picking_On_Object_Local())
    {
        int a = 0;
    }*/
#pragma endregion

    // 플레이어 기본 속도
    m_tPlayer.fSpeed = 5.f;

    return false;
}

void CPlayer::FrameManage(const _float& fTimeDelta)
{
#pragma region 왼손 프레임On
if (!timeline[KEYTYPE_LEFTHAND].empty())
{
    // 왼손 선형 보간
    LeftInterpolation();

    // 왼손 프레임이 켜졌을 때만
    if (m_tLeftHand.bLeftFrameOn)
    {

        // 라이터 되돌리기가 켜져있을 경우
        if (bBackRighter)
        {
            LeftLoadAnimationFromFile("BackRighter"); // 되돌리는 애니메이션
        }
        // 현재 시간
        m_tTime.fLeftCurrentTime += m_tTime.fLeftChangeTime * fTimeDelta;

        // 현재 프레임이 최대 프레임에 도달한 경우
        if (m_tTime.fLeftCurrentTime >= m_tTime.fLeftMaxTime)
        {
            // 꺼내는 모션중일 경우
            if (m_tLeftHand.bPickUpState)
            {
                m_tLeftHand.bPickUpState = false;
            }
            else // 꺼내는 모션이 아닐 경우
            {
                // 왼손 프레임과 시간 초기화
                m_tLeftHand.fLeftFrame = 0.f;
                m_tTime.fLeftCurrentTime = 0.f;
                m_tLeftHand.bLeftFrameOn = false;

                // 양손이 주먹인데 프레임이 최대 프레임에 도달한 경우
                if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                    m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
                {
                    if (m_bAttack && bLeftPunch)
                    {
                        // 왼손 프레임과 시간 초기화
                        m_tLeftHand.fLeftFrame = 0.f;
                        m_tTime.fLeftCurrentTime = 0.f;

                        bLeftPunch = false;
                        bRightPunch = true;

                        m_bAttack = false;
                        //m_tLeftHand.bLeftAttacColOn = true;
                    }
                }

                if (m_bAttack)
                {
                    m_bAttack = false;
                    m_tLeftHand.bLeftAttacColOn = true;
                }
            }

            // 만약 최대프레임인데 라이터가 켜져있을 경우
            if (bRighter)
            {
                // (현재 프레임) 라이터를 켜져있는 이미지(마지막)로 고정
                m_tLeftHand.fLeftFrame = _float(timeline[KEYTYPE_LEFTHAND].back().texureframe);
                m_PlayerLighter = true;
                //m_tLeftHand.bLeftFrameOn = false;
            }
            else // 라이터가 안켜져있을 경우
            {
                // 현재 프레임 초기화
                m_tLeftHand.fLeftFrame = 0.f;
                m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
            }
            // 라이터 되돌리기가 켜져있을 경우
            if (bBackRighter)
            {
                bRighter = false;
                bBackRighter = false;
                m_PlayerLighter = false;
                m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE); // 왼손 상태 초기화
            }
        }
    }
}
#pragma endregion

#pragma region 오른손 프레임On (왼손과 동일)
if (!timeline[KEYTYPE_RIGHTHAND].empty())
{
    // 오른손 선형 보간
    RightInterpolation();

    // 오른손 프레임이 켜졌을 때만
    if (m_tRightHand.bRightFrameOn)
    {
        // 현재 시간
        m_tTime.fRightCurrentTime += m_tTime.fRightChangeTime * fTimeDelta;

        // 차징 공격이 켜지고, 현재 프레임이 풀 차징 프레임에 도달했을 경우
        if (bChargeAttack &&
            m_tTime.fRightCurrentTime >= timeline[KEYTYPE_RIGHTHAND][m_tRightHand.iFullChargingIndex].time)
        {
            // 현재 시간과 텍스처 이미지를 풀 차징에 고정
            m_tRightHand.fRightFrame = _float(timeline[KEYTYPE_RIGHTHAND][m_tRightHand.iFullChargingIndex].texureframe);
            m_tTime.fRightCurrentTime = timeline[KEYTYPE_RIGHTHAND][m_tRightHand.iFullChargingIndex].time;
        }

        // 플레이어 발차기 중 프레임이 다 돌았을 경우
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::KICK &&
            m_tTime.fRightCurrentTime >= m_tTime.fRightMaxTime)
        {
            m_tPlayer_State.Set_State(STATE_PLAYER::NONE); // 플레이어 상태 초기화
            Two_Hand(); // 맨 주먹으로 돌아가기 (나중에 이전상태로 돌아가게 해야함)
        }

        // 오른손 프레임이 최대 프레임에 도달했을 경우
        if (m_tTime.fRightCurrentTime >= m_tTime.fRightMaxTime)
        {
            // 만약 최대프레임인데 버리는 중일 경우
            if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
            {
                // (현재 프레임) 오른손을 Throw 이미지로 고정
                m_tRightHand.fRightFrame = _float(timeline[KEYTYPE_RIGHTHAND].back().texureframe);

                // 시간마다 텍스처 변경 시간을 채워준다.
                m_tTime.fRightCurrentTime += m_tTime.fRightChangeTime * fTimeDelta;
            }
            else // 버리는 중이 아닐 경우
            {
                // 플레이어 차징 공격 중일 경우
                if (m_tRightHand.bChargingAttackOn)
                {
                    m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::IDLE); // 플레이어 행동 : 초기화

                    m_bAttack = false;
                    m_tRightHand.bBasicAttackOn = true; // 기본 공격으로 되돌리기
                    m_tRightHand.bChargingAttackOn = false;
                    m_tRightHand.bRightAttacColOn = true;
                }
                else
                {
                    // 양손이 주먹 상태 일경우
                    if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                        m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
                    {
                        // 오른손 프레임이 다 돌았을 경우
                        if (m_bAttack && bRightPunch)
                        {
                            // 오른손 프레임과 시간 초기화
                            m_tRightHand.fRightFrame = 0.f;
                            m_tTime.fRightCurrentTime = 0.f;

                            m_bAttack = false;
                            //m_tRightHand.bRightAttacColOn = true;

                            bLeftPunch = true;
                            bRightPunch = false;
                        }
                    }
                }
                // 꺼내는 모션중일 경우
                if (m_tRightHand.bPickUpState)
                {
                    m_tRightHand.bPickUpState = false;
                }
                else // 꺼내는 모션이 아닐 경우
                {
                    // 오른손 프레임과 시간 초기화
                    m_tRightHand.fRightFrame = 0.f;
                    m_tTime.fRightCurrentTime = 0.f;
                    m_tRightHand.bRightFrameOn = false;

                    if (m_bAttack)
                    {
                        m_bAttack = false;
                        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::BEERBOTLE)
                        {
                            m_tRightHand.bRightAttacColOn = true;
                        }
                        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::FRYINGPAN)
                        {
                            m_tRightHand.bRightAttacColOn = true;
                        }
                    }
                }
            }
        }
    }
}
#pragma endregion
}

void CPlayer::Charge(const _float& fTimeDelta)
{

}

_bool CPlayer::Picking_On_Object()
{
    CRcBufferComp* pObjectBufferCom = dynamic_cast<CRcBufferComp*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Food1", L"Proto_RcTexBufferComp"));
    NULL_CHECK_RETURN(pObjectBufferCom, _vec3());
    CTransformComponent* pObjectTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Food1", L"Proto_TransformComp"));
    NULL_CHECK_RETURN(pObjectTransCom, _vec3());

    _vec3 PickingPos = m_pCalculatorComp->Picking_On_Object_Local(g_hWnd, pObjectBufferCom, pObjectTransCom);

    if (PickingPos)
        return true;
    else
        return false;
}

CGameObject* CPlayer::RayCast()
{
    CDynamicCamera* pCamera = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Camera", L"DynamicCamera"));

    if (pCamera)
    {
        _vec3 vTest = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vTest, &vTest);

        _vec3 vPos = m_pTransformComp->Get_Pos();
        vPos.y += 0.7f;

        /*auto listMonster = Engine::IntersectTests_Line_GetGameObject(0, m_pTransformComp->Get_Pos(),
            m_pTransformComp->Get_Pos() + vTest * 100.f,
            LAYER_MONSTER);*/
        auto listMonster = Engine::IntersectTests_Ray_GetGameObject(0, vPos,
            m_pTransformComp->Get_Look(),
            _ulong(-1) ^ LAYER_PLAYER);

        if (!listMonster.empty())
        {
            CGameObject* pDstObj = nullptr;
            _float fDist = FLT_MAX;

            for (auto iter = listMonster.begin(); iter != listMonster.end(); ++iter)
            {
                _vec3 vPos = (*iter).second.vContactPoint.Convert_DX9Vec3();
                _float fLength = D3DXVec3Length(&(m_pTransformComp->Get_Pos() - vPos));
                if (fLength < fDist)
                {
                    fDist = fLength;
                    pDstObj = (*iter).first;
                }
            }


            if (pDstObj)
            {
                cout << " 거리 : " << fDist << endl;
                CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDstObj);
                // 타겟 지정
                //CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

                //cout << " 체력 : " << pMonster->Get_MonsterHP().Cur << endl;

                return pDstObj;
            }
            else
            {
                return nullptr;
            }
        }

    }

    return nullptr;
}

void CPlayer::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
    // 충돌중일때
   // OutputDebugString(L"플레이어와 충돌중\n");
    
    CSolid* pSolid = dynamic_cast<CSolid*>(pDst);
    if (nullptr != pSolid)
    {
        _vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
        m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
        if (D3DXVec3Dot(&(-vNormal), &_vec3({ 0.f, -1.f, 0.f })) < 0.f)
            m_IsOnGround = true;
    }

    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr != pAceObj)
    {
        if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // 적대관계
        {
            CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

            if (pMonster != nullptr && !pMonster->Get_IsMonsterDeath() 
            && !m_tPlayer_Action.IsOnState(STATE_PLAYER_ACTION::RUN))
            {
                _vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
                m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
            }
        }
    }
}

void CPlayer::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;

    //플레이어 Hostile관계 =  Monster , Boss
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // 적대관계
    {
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

        if (pMonster == nullptr) 
        {
            CMonsterAttackUnion* pMonsterSkill = dynamic_cast<CMonsterAttackUnion*>(pAceObj);

            if (nullptr == pMonsterSkill)
                return;
            else
            {
                Engine::Add_GameObject(L"UI", CUI_PlayerHurt::Create(m_pGraphicDev));

                if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (4).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (5).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (6).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (7).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (8).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (9).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else if (Random_variable(10))
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (10).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
                else
                    Engine::Play_Sound(L"FallenAces", L"PlayerHurt (11).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

                // 충돌한게 몬스터의 스킬일때  진행하는곳 
                m_bHitState = true;
            }

        }
        else
        {
            /*if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (4).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (5).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (6).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (7).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (8).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (9).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else if (Random_variable(10))
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (10).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            else
                Engine::Play_Sound(L"FallenAces", L"PlayerHurt (11).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);*/

            // 충돌한게 몬스터일때 진행하는곳
            switch (m_eRIGHTState)
            {
            case Engine::STATE_RIGHTHAND::RUN_HAND:
                m_bHitState = true;
                break;
            case Engine::STATE_RIGHTHAND::KICK:
                m_bHitState = true;
                break;
            }
        }
    }


    //// 처음 충돌했을때
    //CBrown* pBrown = dynamic_cast<CBrown*>(pDst->Get_Owner());
    //if (FAILED(pBrown))
    //{
    //    CGray* pBrown = dynamic_cast<CGray*>(pDst->Get_Owner());
    //    OutputDebugString(L"플레이어 - Gray  충돌중\n");
    //}
    //else
    //{
    //    OutputDebugString(L"플레이어 - Brown  충돌중\n");
    //}
}

void CPlayer::OnCollisionExited(CGameObject* pDst)
{
    m_bHitState = false;
    // 충돌에서 나갈때
   // OutputDebugString(L"플레이어와 충돌완료\n");
}

void CPlayer::CrossHairState()
{
    //블랙보드 업로드용 변수 없어서 만드려고 


}

// 블랙보드 정보 업데이트 (정보 올려주기)
void CPlayer::Update_BlackBoard()
{
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // 연결 실패
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 업데이트 한다.
    pBlackBoard->Get_HP().Cur = m_gHp.Cur;
    pBlackBoard->Get_GunLight() = m_bGunLight;
    pBlackBoard->Get_LighterLight() = m_PlayerLighter;
    pBlackBoard->Set_PlayerState(m_tPlayer_State.Get_State());
    pBlackBoard->Set_RightHandState(m_tRightHand_State.Get_State());
    pBlackBoard->Get_AttackOn() = m_bAttack;
    pBlackBoard->Get_PlayerHit() = m_bHitState;

    m_bMagicBottle = pBlackBoard->Get_MagicBottle();


    switch (m_bRAttackMove)
    {
    case EBLACKBOARD_UPLOAD:
        pBlackBoard->Get_RAttackMove() = true;
        m_bRAttackMove = EBLACKBOARD_DOWNLOAD;
        break;
    case EBLACKBOARD_DOWNLOAD:
        m_bRAttackMove = (pBlackBoard->Get_RAttackMove()) ? m_bRAttackMove : EBLACKBOARD_NONE;
        break;
    default:
        break;
    }

    //if (m_bRAttackMove)
    //{
    //    pBlackBoard->Get_RAttackMove() = true;
    //    m_bRAttackMove = false;
    //}

    switch (m_bLAttackMove)
    {
    case EBLACKBOARD_UPLOAD:
        pBlackBoard->Get_LAttackMove() = true;
        m_bLAttackMove = EBLACKBOARD_DOWNLOAD;
        break;
    case EBLACKBOARD_DOWNLOAD:
        m_bLAttackMove = (pBlackBoard->Get_LAttackMove()) ? m_bLAttackMove : EBLACKBOARD_NONE;
        break;
    default:
        break;
    }
        

    //pBlackBoard->Get_PlayerPickUpState() = m_tRightHand.; //차징상태
    //pBlackBoard->Get_PlayerEquipGunState() = m_tRightHand.; //차징상태


}

// 블랙보드 정보 내려받기 (정보 내려받기)
void CPlayer::Update_InternalData()
{
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Camera.Get_BlackBoard())
    {
        m_wpBlackBoard_Camera.Set_BlackBoard(Engine::Get_BlackBoard(L"Camera"));
        // 연결 실패
        if (!m_wpBlackBoard_Camera.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Camera* pBlackBoard = m_wpBlackBoard_Camera.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 얻어온다.
    m_tCamera.bOne = pBlackBoard->Get_OneSwitch();
    m_tCamera.bThree = pBlackBoard->Get_ThreeSwitch();
    m_tCamera.vEye = pBlackBoard->Get_Eye();
    m_tCamera.vAt = pBlackBoard->Get_At();
}

bool CPlayer::Attack_Input(const _float& fTimeDelta)
{
    //발차기
   //if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
   //{
   //    m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK);
   //}

#pragma region 마우스

    // 마우스 좌클릭 (누르고 있을 때)
    if (Engine::IsMouse_Pressing(DIM_LB))
    {
        if (!m_bAttack) // 차징이 가능한 상태일 때만
        {
            switch (m_eRIGHTState)
            {
            case STATE_RIGHTHAND::NONE:
            {
                m_fChage.Cur = 0.f;
                break;
            }
            case STATE_RIGHTHAND::RUN_HAND:
            {
                m_fChage.Cur = 0.f;
                break;
            }
            case STATE_RIGHTHAND::GUN:
            {
                m_fChage.Cur = 0.f;
                //m_tRightHand.bRightAttacColOn = true;
                break;
            }
            case STATE_RIGHTHAND::THOMPSON:
            {
                m_tRightHand.bRightFrameOn = true;
                m_bAttack = true;
                m_bRAttackMove = true;
                m_fChage.Cur = 0.f;
                //m_tRightHand.bRightAttacColOn = true;
                break;
            }

                case STATE_RIGHTHAND::STEELPIPE:
                {
                    // 차지를 시작할 시간
                    if ((m_fChage.Update(1.f * fTimeDelta, m_tTime.fChargeStartTime)))
                    {
                        m_tRightHand.bRightFrameOn = true;

                        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::CHARGING);
                    }
                    break;
                }
                case STATE_RIGHTHAND::FRYINGPAN:
                {
                    // 차지를 시작할 시간
                    if ((m_fChage.Update(1.f * fTimeDelta, m_tTime.fChargeStartTime)))
                    {
                        m_tRightHand.bRightFrameOn = true;

                        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::CHARGING);
                    }
                    break;
                }
                case STATE_RIGHTHAND::HAND:
                {
                    // 차지를 시작할 시간
                    if ((m_fChage.Update(1.f * fTimeDelta, m_tTime.fChargeStartTime)))
                    {
                        m_tRightHand.bRightFrameOn = true;

                        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::CHARGING);
                    }
                        break;
                }

            case STATE_RIGHTHAND::BEERBOTLE:
            {
                m_fChage.Cur = 0.f;
                break;
            }
            case STATE_RIGHTHAND::KICK:
            {
                m_fChage.Cur = 0.f;
                break;
            }
            }

        }
    }

    // 차징이 안켜졌고, 꺼내는 중이 아닐 경우
    if (!m_tRightHand.bPickUpState &&
        m_tPlayer_Action.Get_State() != STATE_PLAYER_ACTION::CHARGING)
    {
        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::ATTACK);

        if (Engine::IsMouse_Pressed(DIM_LB)) // 눌렀을 때
        {
            m_bAttack = true; // 공격 On

            // 앉은 채로 공격을 눌렀을 때
            if (m_tPlayer_State.Get_State() == STATE_PLAYER::SITDOWN)
            {
                m_eAttackState = PSITDONW_ATTACK; // 앉아서 공격
            }
            else if (false) // 추 후 다른상태의 공격 추가
            {

            }
            else // 특정 상태가 없을 경우 노말공격 상태
            {
                m_eAttackState = PNOMAL_ATTACK;
            }
                
            // 양손이 주먹 상태 일경우
            if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
            {
                if (bLeftPunch)
                {
                    m_tLeftHand.bLeftFrameOn = true;
                    m_tLeftHand.bLeftAttacColOn = true;
                    m_bLAttackMove = true;
                }
                if (bRightPunch)
                {
                    m_tRightHand.bRightFrameOn = true;
                    m_tRightHand.bRightAttacColOn = true;
                    m_bRAttackMove = true;
                }
            }
            else // 나머지 공격
            {
                // 플레이어 상태 : 공격, 플레이어 오른손 프레임 On
                //m_tPlayer_State.Set_State(STATE_PLAYER::ATTACK);
                m_tRightHand.bRightFrameOn = true;
                //m_tRightHand.bRightAttacColOn = true;
                m_bRAttackMove = true;

                if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::GUN)
                {
                    m_tRightHand.bRightAttacColOn = true;
                }
            }
        }
    }

    // 마우스 좌클릭 (뗄 때)
    if (Engine::IsMouse_Released(DIM_LB))
    {
        m_fChage.Cur = 0.f;
    }

    // 마우스 우클릭
    if (Engine::IsMouse_Pressing(DIM_RB))
    {
        // 쉴드 가능 일때
        if (timeline[KEYTYPE_RIGHTHAND].front().bShieldPossible)
        {
            // 현재 프레임이 0일 때
            if (m_tRightHand.fRightFrame == 0)
            {
                // 쉴드On
                bShieldOn = true;

                // 양손이 주먹 상태 일경우
                if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                    m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
                {

                }
            }
        }
    }
    else
    {
        // 쉴드Off
        bShieldOn = false;
    }
#pragma endregion

    return false;
}

// 마우스 움직임
void CPlayer::Mouse_Move()
{
#pragma region 1인칭
    if (m_tCamera.bOne)
    {
        //m_pTransformComp->Rotate(ROT_X, m_pCamera->Get_At());

        //마우스로 플레이어 회전
        //상, 하
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformComp->Rotate(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
        }
        // 좌, 우
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
        {
            m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
        }
    }
#pragma endregion

#pragma region 3인칭 쿼터니언
    if (m_tCamera.bThree)
    {
        _vec3   CameraToPlayer;
        _vec3	vPlayerPos;
        _vec3	vLook = { 0.f, 0.f, 1.f };
        _float	fDot;
        //_vec3	m_vPlayerLook;
        //_vec3	m_vCameraAt;

        m_pTransformComp->Get_Info(INFO_POS, &vPlayerPos);

        CameraToPlayer = vPlayerPos - m_tCamera.vEye;

        // 카메라가 바라보는 At
        //m_vCameraAt = m_pCamera->Get_At();

        // 플레이어가 바라보는 Look
        //m_pTransformComp->Get_Info(INFO_LOOK, &m_vPlayerLook);

        // 카메라와 플레이어가 바라보는 높이를 똑같이 맞춰준다.
        CameraToPlayer.y = 0.f;
        //m_vPlayerLook.y = 0.f;

        // 카메라와 플레이어 벡터를 정규화
        D3DXVec3Normalize(&CameraToPlayer, &CameraToPlayer);
        D3DXVec3Normalize(&vLook, &vLook);

        float fAngle;


        if (CameraToPlayer.x > 0)
        {
            // 내적을 이용해 둘 사이 끼인각을 구해준다.
            fDot = D3DXVec3Dot(&CameraToPlayer, &vLook);
            // 두 벡터 사이의 각도 계산 (라디안)
            fAngle = acosf(fDot);
        }
        else
        {
            //vLook = vLook * -1;
            // 내적을 이용해 둘 사이 끼인각을 구해준다.
            fDot = D3DXVec3Dot(&CameraToPlayer, &(vLook * -1));
            // 두 벡터 사이의 각도 계산 (라디안)
            fAngle = acosf(fDot);

            fAngle = fAngle + 3.141592f;
        }

        m_pTransformComp->Set_ScaleY(fAngle);
    }
#pragma endregion

#pragma region 1인칭 쿼터니언 회전 (폐기)
    //dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
    //dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

    //D3DXQUATERNION quatPitch, quatYaw, quatRoll;

    //// 플레이어의 Up 벡터를 가져온다.
    //m_pTransformComp->Get_Info(INFO_UP, &m_vUp);

    //// 플레이어의 Look 벡터를 가져온다.
    //m_pTransformComp->Get_Info(INFO_LOOK, &m_vLook);

    //// 가져온 플레이어의 Up, Look 벡터로 Right벡터 구하기
    //D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);

    //// 수평 회전 (Y축 회전)
    //if (dwMouseMoveX != 0)
    //{
    //    // 마우스 X 이동에 따라 Y축 주위로 회전하는 쿼터니언 계산
    //    D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));

    //    // 현재 쿼터니언에 새로운 회전을 곱하여 누적시킨다.
    //    m_quaternion *= quatYaw;

    //    // 누적된 회전 쿼터니언 정규화
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// 수직 회전 (X축 회전)
    //if (dwMouseMoveY != 0)
    //{
    //    // 마우스 X 이동에 따라 Y축 주위로 회전하는 쿼터니언 계산
    //    D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));

    //    // 현재 쿼터니언에 새로운 회전을 곱하여 누적시킨다.
    //    m_quaternion *= quatPitch;

    //    // 누적된 회전 쿼터니언 정규화
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// 쿼터니언을 사용하여 변환 행렬을 계산하기
    //D3DXMATRIX matRotation;
    //D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion);

    //// 행렬을 플레이어의 변환에 적용하기.
    //m_pTransformComp->Set_WorldMatrix(matRotation);   
#pragma endregion
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CPlayer::Free()
{
    SUPER::Free();
}

void CPlayer::PlayerJump(float fTimeDelta)
{
    if (!m_IsOnGround)
        m_vSpeed += m_vAccel * fTimeDelta;

    m_pTransformComp->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);
}

void CPlayer::Height_On_Terrain()
{
    _vec3		vPos = m_pTransformComp->Get_Pos();

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
    if (nullptr == pTerrainBufferComp)
        return;

    _float	fHeight = m_pCalculatorComp->
        Compute_HeightOnTerrain(&vPos, 
                                pTerrainBufferComp->Get_VtxPos(), 
                                pTerrainBufferComp->Get_VertexCountX() + 1U, 
                                pTerrainBufferComp->Get_VertexCountZ() + 1U,
                                pTerrainBufferComp->Get_Scale(),
                                pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.7f, vPos.z);
}

void CPlayer::Dash(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vRight;
    m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
    D3DXVec3Normalize(&vRight, &vRight);

    _vec3 vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    _vec3 vSpeed = { 0.f, -m_tDash.fDownDash, 0.f };

    float fDashHeight = 4.0f; // 대쉬 높이 무빙 조절 (빠르기)

    // 대쉬 여부
    if (bDashOn)
    {
        // 대쉬 높이 최대치
        if (m_tDash.fDownDash >= 12.f)
        {
            bDashChange = true; // 감소 시작
        }

        if (bDashChange)
        {
            m_tDash.fDownDash -= fDashHeight * 0.2f; // 다운 수치 감소
        }
        else
        {
            m_tDash.fDownDash += fDashHeight * 0.2f; // 다운 수치 증가
        }

        float fTime = 1.f * fTimeDelta;

        // 대쉬 높이
        m_pTransformComp->Set_MovePos(fTime, vSpeed);

        // 대쉬 속도가 스피드보다 빠를 경우만 작동
        if (m_tDash.fDash > m_tPlayer.fSpeed)
        {
            // 대쉬 방향 및 이동
            switch (m_eDashDir)
            {
            case CPlayer::LEFT:
                m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, m_tDash.fDash);
                break;
            case CPlayer::RIGHT:
                m_pTransformComp->Move_Pos(&vRight, fTimeDelta, m_tDash.fDash);
                break;
            case CPlayer::DOWN:
                m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -m_tDash.fDash);
                break;
            }
            m_tDash.fDash--; // 대쉬 속도 감소
        }
        else // 대쉬 속도가 스피드와 같으면
        {

        }

        if (m_tDash.fDownDash <= 0.f)
        {
            bDashChange = false;
            bDashOn = false; // 대쉬 정지
        }
    }

}

#pragma region 양손 주먹 (기본 상태)
void CPlayer::Two_Hand()
{
    // 최대 프레임 설정
    //m_fLeftMaxFrame = 2.f;
    //m_fRightMaxFrame = 2.f;

    m_eObjectName = OBJECT_NAME::NONE; // 오브젝트 없음

    // 오른손 주먹
    m_eRIGHTState = STATE_RIGHTHAND::HAND;
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);

    // 라이터를 안켰을 경우
    if (!bRighter)
    {
        // 왼손 주먹
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
    }

}
#pragma endregion

#pragma region 양손 오브젝트
void CPlayer::Two_Object()
{
    // 라이터가 켜져있는 상태로 양손 오브젝트에 들어왔을 경우
    if (bRighter)
    {
        // 왼손 현재 프레임 초기화
        m_tLeftHand.fLeftFrame = 0.f;
        bRighter = false;       // 라이터 끄기
    }

    switch (m_eObjectName)
    {
        // 톰슨 기관총
    case CPlayer::OBJECT_NAME::THOMPSON:
    {
        // 오른손 톰슨 기관총 (양손)
        m_eRIGHTState = STATE_RIGHTHAND::THOMPSON;
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::THOMPSON);
        // 왼손 없음
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
        break;
    }
    }
}
#pragma endregion

#pragma region 한손 오브젝트
void CPlayer::Right_Object()
{
    // 오른손
    switch (m_eObjectName)
    {
        // 권총
        case CPlayer::OBJECT_NAME::GUN:
        {
            // 오른손 권총 (한손)
            m_eRIGHTState = STATE_RIGHTHAND::GUN;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
            // 왼손 없음
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
            break;
        }
        // 쇠파이프
        case CPlayer::OBJECT_NAME::STEELPIPE:
        {
            // 오른손 쇠파이프
            m_eRIGHTState = STATE_RIGHTHAND::STEELPIPE;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::STEELPIPE);
            // 왼손 오픈 핸드
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        // 맥주병
        case CPlayer::OBJECT_NAME::BEERBOTLE:
        {
            // 오른손 맥주병
            m_eRIGHTState = STATE_RIGHTHAND::BEERBOTLE;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::BEERBOTLE);
            // 왼손 오픈 핸드
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        // 프라이팬
        case CPlayer::OBJECT_NAME::FRYINGPAN:
        {
            // 오른손 프라이팬
            m_eRIGHTState = STATE_RIGHTHAND::FRYINGPAN;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::FRYINGPAN);
            // 왼손 오픈 핸드
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
    }

    // 왼손
    if (!bRighter) // 라이터를 안켰을 경우
    {
        switch (m_tLeftHand_State.Get_State()) // 현재 왼손 상태
        {
        case CPlayer::STATE_LEFTHAND::NONE: // 왼손 없음
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
            break;
        }
        case CPlayer::STATE_LEFTHAND::HAND: // 왼손 주먹
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
            break;
        }
        case CPlayer::STATE_LEFTHAND::OPEN_HAND: // 왼손 오픈 핸드
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        }
    }
}
#pragma endregion

// 손 상태 확인
void CPlayer::Hand_Check()
{
    // 왼손 오른손 출력 On
    m_tLeftHand.bLeftHandOn = true;
    bRightHandOn = true;
    
    // 플레이어의 손이 기존 상태가 아닐경우
    if (m_eRightState_Old != m_tRightHand_State.Get_State())
    {
        m_tTime.fLeftCurrentTime = 0.f;
        m_tTime.fRightCurrentTime = 0.f;

        m_tTime.fLeftChangeTime = 1.f;         // 프레임 속도 조절
        m_tTime.fRightChangeTime = 1.f;         // 프레임 속도 조절
        bRightGetAnimation = true; // 새로운 애니메이션 불러오기 On
        
        m_tRightHand.bRightFrameOn = true;
        m_tRightHand.bPickUpState = true;
        m_tLeftHand.bPickUpState = true;

        // 새로운 상태가 권총일 경우
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::GUN)
        {

        }

        // 새로운 상태가 주먹일 경우
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
        {
            m_tLeftHand.bLeftFrameOn = true;
            bLeftGetAnimation = true;
        }

        // 새로운 상태가 뛰는중일 경우
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::RUN_HAND)
        {
            m_tLeftHand.bLeftFrameOn = true;
            bLeftGetAnimation = true;
            m_tLeftHand.bPickUpState = true;
        }

        //if (m_eLeftState_Old != m_tLeftHand_State.Get_State())
        {
            m_tLeftHand.bLeftFrameOn = true;
            bLeftGetAnimation = true;
            m_tLeftHand.bPickUpState = true;
        }
    }

    // 플레이어의 현재 상태를 저장
    m_eRightState_Old = m_tRightHand_State.Get_State();
    m_eLeftState_Old = m_tLeftHand_State.Get_State();

    switch (m_eObjectName)
    {
    case CPlayer::OBJECT_NAME::NONE:
        m_eObjectType = OBJECT_TYPE::TWO_HAND;
        break;
    case CPlayer::OBJECT_NAME::GUN:
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        break;
    case CPlayer::OBJECT_NAME::THOMPSON:
        m_eObjectType = OBJECT_TYPE::TWO_OBJECT;
        break;
    case CPlayer::OBJECT_NAME::STEELPIPE:
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        break;
    case CPlayer::OBJECT_NAME::BEERBOTLE:
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        break;
    case CPlayer::OBJECT_NAME::FRYINGPAN:
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        break;
    }

    // 플레이어가 안뛰고있는 경우
    if (m_tPlayer_Action.Get_State() != STATE_PLAYER_ACTION::RUN)
    {
        // 플레이어가 오브젝트를 버리고있을 경우
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
        {
            // 오브젝트 초기화 (삭제)
            m_eObjectType = OBJECT_TYPE::NONE;
            m_eObjectName = OBJECT_NAME::NONE;
        }

        if (bRighter) // 플레이어가 라이터를 킨 경우
        {
            // 왼손 라이터
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
            bLeftGetAnimation = true;
        }

#pragma region 오브젝트 타입 체크
        if (m_tRightHand_State.Get_State() != STATE_RIGHTHAND::KICK) // 플레이어가 발차기를 안한 경우
        {
            // 오브젝트가 NONE일 경우
            if (m_eObjectType == OBJECT_TYPE::NONE ||
                m_eObjectName == OBJECT_NAME::NONE)
            {
                m_eObjectType = OBJECT_TYPE::TWO_HAND; // 맨손
            }

            switch (m_eObjectType)
            {
                case CPlayer::OBJECT_TYPE::TWO_HAND:        // 양손 주먹
                {
                    Two_Hand();
                    break;
                }
                case CPlayer::OBJECT_TYPE::TWO_OBJECT:      // 양손 오브젝트 (두손 무기)
                {
                    Two_Object();
                    break;
                }
                case CPlayer::OBJECT_TYPE::RIGHT_OBJECT:    // 한손 오브젝트 (한손 무기)
                {
                    Right_Object();
                    break;
                }
            }
        }
        else if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::KICK) // 플레이어가 발차기를 한 경우
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE); // 왼손Off
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK); // 오른손 발차기
        }
#pragma endregion
    }
#pragma region 플레이어가 뛰고 있는 경우
    else if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::RUN)
    {
        m_eRIGHTState = STATE_RIGHTHAND::RUN_HAND;
        // 라이터를 켠 채로 뛰는 경우
        if (bRighter)
        {
            // 왼손 라이터 달리기
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_RIHGTER);
            // 오른손 맨손 달리기
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        // 라이터를 키지 않은 경우
        else
        {
            // 양손 다 맨손 달리기
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
    }
#pragma endregion

}

void CPlayer::State_Update(float fTimeDelta)
{
    // 플레이어가 살아있을 때
    if (m_tPlayer_State.Get_State() != STATE_PLAYER::DIE)
    {
        // 플레이어의 상태
        m_tPlayer_State.Get_StateFunc()(this, fTimeDelta);

        // 플레이어 손 상태 체크
        Hand_Check();

        // 플레이어의 왼손 상태
        m_tLeftHand_State.Get_StateFunc()(this, fTimeDelta);

        // 플레이어의 오른손 상태
        m_tRightHand_State.Get_StateFunc()(this, fTimeDelta);
    }
    else
    {
        // 플레이어 죽음
    }
}

#pragma region 플레이어의 상태
void CPlayer::Idle(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {
        if (true)
        {
            
        }
    }

    if (m_tPlayer_State.Can_Update())
    {
        _float fRange = 9.f;
        _float fSpeed = 0.3f;

        if (m_vIdlePos.y < fRange &&
            !m_bHandSwitch)
        {
            m_vIdlePos.y += fSpeed;
        }
        else if (m_vIdlePos.y >= fRange)
        {
            m_bHandSwitch = true;
        }

        if (m_bHandSwitch)
        {
            m_vIdlePos.y -= fSpeed;
        }

        if (m_vIdlePos.y <= -fRange)
        {
            m_bHandSwitch = false;
        }

        //m_pRightHandComp->Set_Pos(m_vIdlePos);
    }

    if (m_tPlayer_State.IsState_Exit())
    {
        m_vIdlePos.y = 0.f;
    }
}

void CPlayer::Down(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {
        float fPHeight = 4.0f; // 높이 조절

        _vec3 vSpeed = { 0.f, -fPHeight, 0.f };

        float fTime = 1.f * fTimeDelta;
        // 높이
        m_pTransformComp->Set_MovePos(fTime, vSpeed);
    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Kick(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Die(float fTimeDelta)
{
    if (m_tPlayer_State.IsState_Entered())
    {

    }

    if (m_tPlayer_State.Can_Update())
    {

    }

    if (m_tPlayer_State.IsState_Exit())
    {

    }
}

void CPlayer::Action_Idle(float fTimeDelta)
{
    if (m_tPlayer_Action.IsState_Entered())
    {

    }

    if (m_tPlayer_Action.Can_Update())
    {

    }

    if (m_tPlayer_Action.IsState_Exit())
    {

    }
}

void CPlayer::Action_Move(float fTimeDelta)
{
    if (m_tPlayer_Action.IsState_Entered())
    {

    }

    if (m_tPlayer_Action.Can_Update())
    {

    }

    if (m_tPlayer_Action.IsState_Exit())
    {

    }
}

void CPlayer::Action_Sitdown(float fTimeDelta)
{
    if (m_tPlayer_Action.IsState_Entered())
    {

    }

    if (m_tPlayer_Action.Can_Update())
    {

    }

    if (m_tPlayer_Action.IsState_Exit())
    {

    }
}

void CPlayer::Action_Run(float fTimeDelta)
{
    if (m_tPlayer_Action.IsState_Entered())
    {

    }

    if (m_tPlayer_Action.Can_Update())
    {

    }

    if (m_tPlayer_Action.IsState_Exit())
    {

    }
}

void CPlayer::Action_Charging(float fTimeDelta)
{
    if (m_tPlayer_Action.IsState_Entered())
    {

    }

    if (m_tPlayer_Action.Can_Update())
    {

    }

    if (m_tPlayer_Action.IsState_Exit())
    {

    }
}

void CPlayer::Action_ThrowAway(float fTimeDelta)
{
    if (m_tPlayer_Action.IsState_Entered())
    {

    }

    if (m_tPlayer_Action.Can_Update())
    {

    }

    if (m_tPlayer_Action.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region 플레이어의 왼손 상태

void CPlayer::Left_None(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 왼손 출력 Off
        m_tLeftHand.bLeftHandOn = false;
        // 왼손 현재 프레임 초기화
        //m_fLeftFrame = 0.f;
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }

}

void CPlayer::Left_Hand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 오른손 주먹 이미지
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
        //m_fLeftMaxFrame = 2.f;  // 최대 프레임 설정
        bRighter = false;       // 라이터 Off

        // 처음 꺼내는 애니메이션
        if (m_tLeftHand.bPickUpState)
        {
            // 애니메이션 불러오기
            if (bLeftGetAnimation)
            {
                LeftLoadAnimationFromFile("LeftHandPickUp");
            }
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // 플레이어가 버리는 중 일경우
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
        {
            m_tLeftHand.bLeftHandOn = false; // 왼손Off
        }

        // 꺼내는 애니메이션이 다 돌았을 경우
        if (!m_tLeftHand.bPickUpState)
        {
            if (bLeftGetAnimation)
            {
                m_tLeftHand.bLeftFrameOn = false;      // 왼손 프레임 매니저 Off
                m_tLeftHand.bPickUpState = false;         // 현재 프레임 초기화
                m_tLeftHand.fLeftFrame = 0.f;
                m_tTime.fRightChangeTime = 1.3f;
                m_tTime.fLeftCurrentTime = 0.f;        // 현재 시간 초기화

                // 왼손 주먹 불러오기
                m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"LeftFist");
                LeftLoadAnimationFromFile("LeftFist");

                bLeftGetAnimation = false; // Off
            }
        }

        if (m_bLAttackMove)
        {
            m_bLRotStart = true;
        }

        // 공격시 카메라 무빙
        if (m_bLRotStart)
        {
            fLRotStart += 1.f * fTimeDelta;
            _float fMoveSpeed = 1.5f;

            if (fLRotStart > 0.2f)
            {
                
                if (m_vRotPlus.y < 0.3 && !m_bRotChange)
                {
                    
                    m_vRotPlus.y += fMoveSpeed * fTimeDelta; 
                    m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(m_vRotPlus.y));
                }
                else if (m_vRotPlus.y > 0.3 && !m_bRotChange)
                {
                    //m_vRot.y = 0.f;
                    m_bRotChange = true;
                }
                
                if (m_bRotChange && m_vRotMinus.y > -0.3f)
                {
                    
                    m_vRotMinus.y -= fMoveSpeed * fTimeDelta; 
                    m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(m_vRotMinus.y));
                }

                
                if (m_vRotMinus.y <= -0.3f)
                {
                    m_vRotMinus.y = 0.f;
                    m_vRotPlus.y = 0.f;
                    fLRotStart = 0.f;

                    m_bRotChange = false;
                    m_bLRotStart = false;
                    m_bLAttackMove = false;
                }
            }
        }

        // 공격 생성On
        if (m_tLeftHand.bLeftAttacColOn)
        {
            m_tLeftHand.bLeftAttacColOn = false;    // 공격 생성 Off

            // 공격마다 랜덤 재생
            if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;

            // 주먹공격 생성
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // 레이어, 디바이스
                vPos, m_pTransformComp->Get_Look(),    // 생성위치, 방향
                                   this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // 공격유형, 팀
                                   0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // 속도, 삭제시간, 데미지, 크기
            
            m_bAttack = false;  // 공격 Off
            bLeftPunch = false;
            bRightPunch = true;
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// 왼손 달리기 라이터.ver
void CPlayer::Left_RunRighter(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 라이터On (최대프레임) 그대로, 툴로 위 아래 움직이게 만들어주기
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// 한손 오브젝트일때 왼손 오픈 핸드
void CPlayer::Left_OpenHand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 왼손 오픈 핸드로 변경
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"OpenHand");

        //bGetAnimation = false;
        //m_tLeftHand.fLeftFrame = 0.f;
        //m_tLeftHand.bLeftFrameOn = false;
        //m_tTime.fLeftMaxTime = 0.f;

        // 처음 꺼내는 애니메이션
        if (m_tLeftHand.bPickUpState)
        {
            // 애니메이션 불러오기
            if (bLeftGetAnimation)
            {
                LeftLoadAnimationFromFile("LeftHandPickUp");
            }
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // 꺼내는 애니메이션이 다 돌았을 경우
        if (!m_tLeftHand.bPickUpState)
        {
            if (bLeftGetAnimation)
            {
                m_tLeftHand.bLeftFrameOn = false;      // 왼손 프레임 매니저 Off
                m_tLeftHand.bPickUpState = false;      // 현재 프레임 초기화
                m_tLeftHand.fLeftFrame = 0.f;
                m_tTime.fLeftCurrentTime = timeline[KEYTYPE_LEFTHAND].back().time;        // 현재 시간 초기화

                m_tLeftHand.bLeftFrameOn = false;
                bLeftGetAnimation = false; // Off
            }
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_RunHand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 왼손 뛰는 손으로 변경
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_RunHand");

        // 뛰는 애니메이션
        LeftLoadAnimationFromFile("LeftRun");
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Left_Righter(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // 왼손을 라이터로 변경
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Righter");
        
        // 애니메이션 불러오기
        if (bLeftGetAnimation)
        {
            LeftLoadAnimationFromFile("Righter");
            bLeftGetAnimation = false; // Off
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // 현재 프레임이 최대 프레임보다 크거나 같을 경우(애니메이션을 다 돌았을 때)
        if (m_tTime.fLeftCurrentTime >= m_tTime.fLeftMaxTime)
        {
            if (bRighter)
            {
                // 프레임 재생 Off
                //m_tTime.fLeftCurrentTime = m_tTime.fLeftMaxTime;
            }
            else // 라이터가 꺼졌을 경우
            {
                // 왼손 주먹
                m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
            }
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region 플레이어의 오른손 상태

void CPlayer::Right_None(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_eRightState_Old != m_tRightHand_State.Get_State())
        {

        }
        // 오른손 주먹으로 변경
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);

        // 오른손 출력 Off
        //bRightHandOn = false;

        // 오른손 현재 프레임 초기화
        //m_tRightHand.fRightFrame = 0.f;
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Hand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_tRightHand.bPickUpState)
        {
            // 오른손 주먹 이미지
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("RightHandPickUp");

            }
        }
        //// 플레이어의 상태가 공격일경우 애니메이션 변경
        //if (m_tPlayer_State.Get_State() == STATE_PLAYER::ATTACK)
        //{
        //    // 애니메이션 불러오기
        //    if (bGetAnimation)
        //    {
        //        RightLoadAnimationFromFile("RightFist");
        //        bGetAnimation = false; // Off
        //    }
        //}
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 꺼내는 애니메이션이 다 돌았을 경우
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // 오른손 프레임 매니저 Off
                m_tRightHand.fRightFrame = 0.f;         // 현재 프레임 초기화
                m_tTime.fRightCurrentTime = 0.f;        // 현재 시간 초기화
                m_tTime.fRightChangeTime = 1.3f;
                // 오른손 주먹 불러오기
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");
                RightLoadAnimationFromFile("RightFist");

                bRightGetAnimation = false; // Off
            }
        }

        if (m_bRAttackMove)
        {
            m_bRotStart = true;
        }

        // 공격시 카메라 무빙
        if (m_bRotStart)
        {
            fRotStart += 1.f * fTimeDelta;
            _float fMoveSpeed = 1.5f;

            if (fRotStart > 0.2f)
            {
                if (m_vRotMinus.y > -0.3 && !m_bRotChange)
                {
                    m_vRotMinus.y -= fMoveSpeed * fTimeDelta;
                    m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(m_vRotMinus.y));
                }
                else if(m_vRotMinus.y < -0.3 && !m_bRotChange)
                {
                    //m_vRot.y = 0.f;
                    m_bRotChange = true;
                }

                if (m_bRotChange && m_vRotPlus.y < 0.3f)
                {
                    m_vRotPlus.y += fMoveSpeed * fTimeDelta;
                    m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(m_vRotPlus.y));
                }

                if (m_vRotPlus.y >= 0.3f)
                {
                    m_vRotMinus.y = 0.f;
                    m_vRotPlus.y = 0.f;
                    fRotStart = 0.f;

                    m_bRotChange = false;
                    m_bRotStart = false;
                    m_bRAttackMove = false;
                }
            }
        }

        // 공격 생성On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // 공격 생성 Off

            // 공격마다 랜덤 재생
            if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"Punch (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;
            // 주먹공격 생성
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // 레이어, 디바이스
                vPos, m_pTransformComp->Get_Look(),    // 생성위치, 방향
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // 공격유형, 팀
                0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // 속도, 삭제시간, 데미지, 크기

            m_bAttack = false;  // 공격 Off
            bLeftPunch = true;
            bRightPunch = false;
        }

        // 플레이어가 차징을 하고있을 경우
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::CHARGING)
        {
            bChargeAttack = true;

            // 오른손 프레임 On
            m_tRightHand.bRightFrameOn = true;

            // 차징 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");

            m_tTime.fRightChangeTime = 1.2f;         // 프레임 속도 조절

            // 불러오기 On
            bRightGetAnimation = true;

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("RightHand_Charging");
                bRightGetAnimation = false; // Off

            }

            if (Engine::IsMouse_Released(DIM_LB))
            {
                m_tRightHand.bChargingAttackOn = true;
                m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::ATTACK);
                m_bAttack = true;
                m_bRAttackMove = true;

                bChargeAttack = false;
                m_fChage.Cur = 0.f;
            }

        }

        // 기본 공격 다시 불러오기
        if (m_tRightHand.bBasicAttackOn)
        {
            // 오른손 주먹 다시 불러오기
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

            m_tTime.fRightChangeTime = 1.f;         // 프레임 속도 조절
            RightLoadAnimationFromFile("RightFist");
            m_tRightHand.bBasicAttackOn = false;
        }

        // 플레이어가 버리는 중 일경우
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
        {
            // 버리는 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"UnderThrow_RightHand");
            //m_fRightMaxFrame = 0.f;  // 최대 프레임 지정
            //m_tRightHand.fRightFrameSpeed = 4.f;  // 프레임 속도 지정 (공격 속도)
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_RunHand(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 오른손 뛰는 손으로 변경
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_RunHand");

        // 뛰는 애니메이션
        RightLoadAnimationFromFile("RightRun");
    }

    if (m_tRightHand_State.Can_Update())
    {
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Gun(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_tRightHand.bPickUpState)
        {
            // 오른손 총 회전 이미지
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun_Spin");

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                //Engine::Play_Sound(L"FallenAces", L"GunSpin (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

                RightLoadAnimationFromFile("GunPickUp");
                m_tTime.fRightChangeTime = 1.5f; // 프레임 속도 조절
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 총 회전이 다 돌았을 경우
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // 오른손 프레임 매니저 Off
                m_tRightHand.fRightFrame = 0.f;         // 현재 프레임 초기화
                m_tTime.fRightCurrentTime = 0.f;        // 현재 시간 초기화
                m_tTime.fRightChangeTime = 2.f;         // 프레임 속도 조절

                // 오른손 총 불러오기
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun");
                RightLoadAnimationFromFile("Gun");

                bRightGetAnimation = false; // Off
            }
        }

        if (m_bRAttackMove)
        {
            m_bRotStart = true;
        }

        // 공격시 카메라 무빙
        if (m_bRotStart)
        {
            fRotStart += 1.f * fTimeDelta;
            _float fMoveSpeed = 10.f;

            if (fRotStart > 0.f)
            {

                if (m_vRotPlus.y < 0.3 && !m_bRotChange)
                {

                    m_vRotPlus.y += fMoveSpeed * fTimeDelta;
                    m_pTransformComp->Rotate(ROT_X, D3DXToRadian(m_vRotPlus.y));
                }
                else if (m_vRotPlus.y > 0.3 && !m_bRotChange)
                {
                    //m_vRot.y = 0.f;
                    m_bRotChange = true;
                }

                if (m_bRotChange && m_vRotMinus.y > -0.6f)
                {

                    m_vRotMinus.y -= fMoveSpeed * fTimeDelta;
                    m_pTransformComp->Rotate(ROT_X, D3DXToRadian(m_vRotMinus.y));
                }

                if (m_vRotMinus.y <= -0.6f)
                {
                    m_vRotMinus.y = 0.f;
                    m_vRotPlus.y = 0.f;
                    fRotStart = 0.f;

                    m_bRotChange = false;
                    m_bRotStart = false;
                    m_bRAttackMove = false;
                }
            }
        }

        // 공격 생성On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // 공격 생성 Off

            m_bGunLight = TRUE; // 총 조명On

            //CGameObject* pDst = RayCast();

            // 몬스터 피해  (데미지, 이 공격을 받은 타겟, 이 공격의 유형)
            //RayAttack(pDst , -10.f, m_eAttackState);

            // 공격마다 랜덤 재생
            if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"GunFire (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"GunFire (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"GunFire (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"GunFire (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"GunFire (4).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"GunFire (5).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();

            // 총알 생성
            Engine::Add_GameObject(L"GameLogic", CPlayerBullet::Create(m_pGraphicDev,                // 레이어, 디바이스
                vPos, m_pTransformComp->Get_Look(),    // 생성위치, 방향
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // 공격유형, 팀
                100.f, 1.f, 10.f, 1.f, m_eRIGHTState));                                           // 속도, 삭제시간, 데미지, 크기

            Engine::Add_GameObject(L"GameLogic", CEffect_CartridgeCase::Create(m_pGraphicDev,                // 레이어, 디바이스
                m_pTransformComp->Get_Pos().x + 1.f, m_pTransformComp->Get_Pos().y + 0.5f, m_pTransformComp->Get_Pos().z, 
                m_pTransformComp->Get_Right(), m_pOwner));

            m_bAttack = false;  // 공격 Off
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {
        // 상태가 바껴서 다른 손으로 건너갈 때 내려가는 애니메이션
    }
}

void CPlayer::Right_Thompson(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_tRightHand.bPickUpState)
        {
            // 오른손 톰슨 이미지
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("ThompsonPickUp");
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 톰슨 꺼내는게 다 돌았을 경우
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation) // 애니메이션 변경 On
            {
                m_tRightHand.bRightFrameOn = false;     // 오른손 프레임 매니저 Off
                m_tRightHand.fRightFrame = 0.f;         // 현재 프레임 초기화
                m_tTime.fRightCurrentTime = 0.f;        // 현재 시간 초기화
                m_tTime.fRightChangeTime = 3.f;         // 프레임 속도 조절

                // 오른손 새로운 이미지로 변경
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");
                RightLoadAnimationFromFile("Thompson"); // 새로운 애니메이션 로드

                bRightGetAnimation = false; // 애니메이션 변경 Off
            }
        }

        // 타격생성 속도 조절
        if (m_bAttack)
        {
            fAttackTime += 1.f * fTimeDelta;

            if (fAttackTime > 0.2f)
            {
                m_tRightHand.bRightAttacColOn = true;
                fAttackTime = 0.f;
            }
        }

        if (m_bRAttackMove)
        {
            m_bRotStart = true;
        }

        // 공격시 카메라 무빙
        if (m_bRotStart)
        {
            fRotStart += 1.f * fTimeDelta;
            _float fMoveSpeed = 10.f;

            if (fRotStart > 0.f)
            {
                
                if (m_vRotPlus.y < 0.3 && !m_bRotChange)
                {
                    
                    m_vRotPlus.y += fMoveSpeed * fTimeDelta; 
                    m_pTransformComp->Rotate(ROT_X, D3DXToRadian(m_vRotPlus.y));
                }
                else if (m_vRotPlus.y > 0.3 && !m_bRotChange)
                {
                    //m_vRot.y = 0.f;
                    m_bRotChange = true;
                }
                
                if (m_bRotChange && m_vRotMinus.y > -0.6f)
                {
                    
                    m_vRotMinus.y -= fMoveSpeed * fTimeDelta; 
                    m_pTransformComp->Rotate(ROT_X, D3DXToRadian(m_vRotMinus.y));
                }
                
                if (m_vRotMinus.y <= -0.6f)
                {
                    m_vRotMinus.y = 0.f;
                    m_vRotPlus.y = 0.f;
                    fRotStart = 0.f;

                    m_bRotChange = false;
                    m_bRotStart = false;
                    m_bRAttackMove = false;
                }
            }
        }

        // 공격 생성On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // 공격 생성 Off

            m_bGunLight = TRUE; // 총 조명On

            //CGameObject* pDst = RayCast();

            //// 몬스터 피해  (데미지, 이 공격을 받은 타겟, 이 공격의 유형)
            //RayAttack(pDst, -10.f, m_eAttackState);

            _vec3 vPos = m_pTransformComp->Get_Pos();

            // 사운드 ex)
            // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
            Engine::Play_Sound(L"FallenAces", L"ThompsonFire (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);


            // 총알 생성
            Engine::Add_GameObject(L"GameLogic", CPlayerBullet::Create(m_pGraphicDev, // 레이어, 디바이스
                vPos, m_pTransformComp->Get_Look(),             // 생성위치, 방향
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),   // 공격유형, 팀
                100.f, 1.f, 40.f, 1.f, m_eRIGHTState));                        // 속도, 삭제시간, 데미지, 크기f

            m_bAttack = false;  // 공격 Off
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Steelpipe(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_tRightHand.bPickUpState)
        {
            // 오른손 쇠파이프
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("Steel_PipePickUp");
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 꺼내는 모션이 끝났을 경우
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // 오른손 프레임 매니저 Off
                m_tRightHand.fRightFrame = 0.f;         // 현재 프레임 초기화
                m_tTime.fRightCurrentTime = 0.f;        // 현재 시간 초기화
                m_tTime.fRightChangeTime = 1.3f;

                // 오른손 쇠파이프 불러오기
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");
                RightLoadAnimationFromFile("Steel_Pipe");

                bRightGetAnimation = false; // Off
            }
        }

        // 타격생성 속도 조절
        if (m_bAttack)
        {
            fAttackTime += 1.f * fTimeDelta;

            if (fAttackTime > 0.8f)
            {
                m_tRightHand.bRightAttacColOn = true;
                fAttackTime = 0.f;
            }
        }

        // 공격 생성On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // 공격 생성 Off

            // 공격마다 랜덤 재생
            if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"SteelPipe (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"SteelPipe (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"SteelPipe (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;
            // 파이프 공격 생성
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // 레이어, 디바이스
                vPos, m_pTransformComp->Get_Look(),    // 생성위치, 방향
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // 공격유형, 팀
                0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // 속도, 삭제시간, 데미지, 크기

            m_bAttack = false;  // 공격 Off
        }

        // 플레이어가 차징을 하고있을 경우
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::CHARGING)
        {
            bChargeAttack = true;

            // 오른손 프레임 On
            m_tRightHand.bRightFrameOn = true;

            // 차징 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe_Charging");

            // 불러오기 On
            bRightGetAnimation = true;

            //m_tTime.fRightChangeTime = 1.2f;         // 프레임 속도 조절

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("Steel_Pipe_Charging");
                bRightGetAnimation = false; // Off

            }

            if (Engine::IsMouse_Released(DIM_LB))
            {
                m_tRightHand.bChargingAttackOn = true;
                m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::ATTACK);
                m_bAttack = true;
                m_bRAttackMove = true;

                bChargeAttack = false;
                m_fChage.Cur = 0.f;
            }

        }

        // 기본 공격 다시 불러오기
        if (m_tRightHand.bBasicAttackOn)
        {
            // 오른손 주먹 다시 불러오기
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");
            RightLoadAnimationFromFile("Steel_Pipe");
            m_tRightHand.bBasicAttackOn = false;
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {
        
    }
}

void CPlayer::Right_BeerBotle(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_tRightHand.bPickUpState)
        {
            // 오른손 맥주병
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("Steel_PipePickUp");
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 꺼내는 모션이 다 돌았을 경우
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // 오른손 프레임 매니저 Off
                m_tRightHand.fRightFrame = 0.f;         // 현재 프레임 초기화
                m_tTime.fRightCurrentTime = 0.f;        // 현재 시간 초기화
                m_tTime.fRightChangeTime = 1.5f;
                
                // 오른손 맥주병 불러오기
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");
                RightLoadAnimationFromFile("Beer_Botle");

                bRightGetAnimation = false; // Off
            }
        }

        // 오른손 프레임이 다 돌았을 경우
        if (m_tTime.fRightCurrentTime > m_tTime.fRightMaxTime)
        {
                                        // 속도, 삭제시간, 데미지, 크기
        }

        // 공격 생성On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // 공격 생성 Off

            // 맥주병(폭발 스킬) 사운드는 PlayerLightning에서 재생

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;

            CGameObject* pGmaeObj = nullptr;
            CAceMonster* pAceMonster = nullptr;

            pGmaeObj = RayCast();

            if (pGmaeObj != nullptr)
                pAceMonster = dynamic_cast<CAceMonster*>(pGmaeObj);
            //
           // // 번개 or 폭발 스킬 생성 (라이트닝인데 폭발공격임 정상 작동, 허공에다 공격하면 안나감, 내려쳤을때 기준)
            if (pAceMonster != nullptr)
            {
                if(m_bMagicBottle)
                    Engine::Add_GameObject(L"GameLogic", CPlayerLightning::Create(m_pGraphicDev, pAceMonster)); // 디바이스, 몬스터 
                else
                {                // 맥주병 공격 생성
                    Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // 레이어, 디바이스
                        vPos, m_pTransformComp->Get_Look(),    // 생성위치, 방향
                        this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                       // 공격유형, 팀
                        100.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                            // 속도, 삭제시간, 데미지, 크기

                    m_bAttack = false;  // 공격 Off
                }
            }
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_FryingPan(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 처음 꺼내는 애니메이션
        if (m_tRightHand.bPickUpState)
        {
            // 오른손 프라이팬
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("FryingPanPickUp");
            }
            
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // 총 회전이 다 돌았을 경우
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // 오른손 프레임 매니저 Off
                m_tRightHand.fRightFrame = 0.f;         // 현재 프레임 초기화
                m_tTime.fRightCurrentTime = 0.f;        // 현재 시간 초기화
                m_tTime.fRightChangeTime = 1.3f;

                // 오른손 총 불러오기
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
                RightLoadAnimationFromFile("FryingPan");

                bRightGetAnimation = false; // Off
            }
        }

        // 공격 생성On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // 공격 생성 Off

            // 공격마다 랜덤 재생
            if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"FryingPan (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"FryingPan (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // 사운드 ex)
                // 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
                Engine::Play_Sound(L"FallenAces", L"FryingPan (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;

            // 총알 생성
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // 레이어, 디바이스
                m_pTransformComp->Get_Pos(), m_pTransformComp->Get_Look(),    // 생성위치, 방향
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // 공격유형, 팀
                0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // 속도, 삭제시간, 데미지, 크기

            m_bAttack = false;  // 공격 Off
        }

        // 플레이어가 차징을 하고있을 경우
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::CHARGING)
        {
            bChargeAttack = true;

            // 오른손 프레임 On
            m_tRightHand.bRightFrameOn = true;

            // 차징 텍스처로 변경
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan_Charging");

            // 불러오기 On
            bRightGetAnimation = true;

            // 애니메이션 불러오기
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("FryingPan_Charging");
                bRightGetAnimation = false; // Off

            }

            if (Engine::IsMouse_Released(DIM_LB))
            {
                m_tRightHand.bChargingAttackOn = true;
                m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::ATTACK);
                m_bAttack = true;
                //m_bRAttackMove = EBLACKBOARD_UPLOAD;
                m_bRAttackMove = true;

                bChargeAttack = false;
                m_fChage.Cur = 0.f;
            }
        }

        // 기본 공격 다시 불러오기
        if (m_tRightHand.bBasicAttackOn)
        {
            // 오른손 주먹 다시 불러오기
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
            RightLoadAnimationFromFile("FryingPan");
            m_tRightHand.bBasicAttackOn = false;
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Kick(float fTimeDelta) // @@@안씀@@@
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // 오른손 발차기
        m_eRIGHTState = STATE_RIGHTHAND::KICK;
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Kick");
        //m_fRightMaxFrame = 4.f; // 최대 프레임 지정
        //m_tRightHand.fRightFrameSpeed = 7.f; // 프레임 속도 지정 (공격 속도)
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

// 왼손 애니메이션 불러오기
void CPlayer::LeftLoadAnimationFromFile(const char* fileName)
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때의 오류 처리
        return;
    }

    timeline[KEYTYPE_LEFTHAND].clear();
    KEYFRAME keyframe;

    while (file >>
        keyframe.time >>
        keyframe.value >>
        keyframe.type >>
        keyframe.isEaseIn >>
        keyframe.isEaseOut >>
        keyframe.bChargePossible >>
        keyframe.bShieldPossible >>
        keyframe.texureframe >>
        keyframe.bFullChargeKeyframe >>
        keyframe.bShieldKeyFrame >>
        keyframe.vScale.x >>
        keyframe.vScale.y >>
        keyframe.vScale.z >>
        keyframe.vRot.x >>
        keyframe.vRot.y >>
        keyframe.vRot.z >>
        keyframe.vPos.x >>
        keyframe.vPos.y >>
        keyframe.vPos.z >>
        keyframe.vKeyFramePos.x >>
        keyframe.vKeyFramePos.y)
    {
        timeline[KEYTYPE_LEFTHAND].push_back(keyframe);
    }



    file.close();

#pragma region 마지막 키프레임으로 설정할 값들
    // 최대 프레임 설정
    m_tTime.fLeftMaxTime = timeline[KEYTYPE_LEFTHAND].back().time;
#pragma endregion  
}

// 오른손 애니메이션 불러오기
void CPlayer::RightLoadAnimationFromFile(const char* fileName)
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) 
    {
        // 파일을 열 수 없을 때의 오류 처리
        return;
    }
    
    timeline[KEYTYPE_RIGHTHAND].clear();
    KEYFRAME keyframe;

    while (file >>
        keyframe.time >>
        keyframe.value >>
        keyframe.type >>
        keyframe.isEaseIn >>
        keyframe.isEaseOut >>
        keyframe.bChargePossible >>
        keyframe.bShieldPossible >>
        keyframe.texureframe >>
        keyframe.bFullChargeKeyframe >>
        keyframe.bShieldKeyFrame >>
        keyframe.vScale.x >>
        keyframe.vScale.y >>
        keyframe.vScale.z >>
        keyframe.vRot.x >>
        keyframe.vRot.y >>
        keyframe.vRot.z >>
        keyframe.vPos.x >>
        keyframe.vPos.y >>
        keyframe.vPos.z >>
        keyframe.vKeyFramePos.x >>
        keyframe.vKeyFramePos.y)
    {
        timeline[KEYTYPE_RIGHTHAND].push_back(keyframe);
    }

    file.close();

#pragma region 마지막 키프레임으로 설정할 값들
    // 최대 프레임 설정
    m_tTime.fRightMaxTime = timeline[KEYTYPE_RIGHTHAND].back().time;
#pragma endregion  
}

void CPlayer::LeftInterpolation() // 왼손, 오른손 선형 보간 함수 별개로 만들기
{
    if (!timeline[KEYTYPE_LEFTHAND].empty()) // 해당 키타입이 비었는지 체크
    {
        if (m_tTime.fLeftCurrentTime >= 0.f &&
            m_tTime.fLeftCurrentTime <= timeline[KEYTYPE_LEFTHAND].back().time)
        {
            // 프레임 0으로 초기화
            _uint iFrameIndex = 0U;

            // 사이즈의 끝에서부터 시작해서 찾기
            for (_uint j = _uint(timeline[KEYTYPE_LEFTHAND].size()) - 1; j >= 0; j--)
            {
                if ((timeline)[KEYTYPE_LEFTHAND][j].time <= m_tTime.fLeftCurrentTime)
                {
                    iFrameIndex = j;
                    break;
                }
            }

            // Linear
            if (iFrameIndex + 1U < timeline[KEYTYPE_LEFTHAND].size())
            {
                // 키 프레임간 시간 변화율
                fFrameTimeDelta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].time - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].time;

                // 현재 키 프레임시간부터 현재 시간 변화율
                fCurFrameTimeDelta = (m_tTime.fLeftCurrentTime - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].time);

                fSizeX_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vScale.x - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.x;
                fSizeX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                fSizeY_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vScale.y - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.y;
                fSizeY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                fRotX_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vRot.x - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.x;
                fRotX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                fRotY_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vRot.y - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.y;
                fRotY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                fRotZ_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vRot.z - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.z;
                fRotZ_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                fPosX_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vPos.x - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.x;
                fPosX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                fPosY_Delta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].vPos.y - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.y;
                fPosY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                m_pLeftHandComp->Set_Pos({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.x + fPosX_Delta,
                (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.y + m_vIdlePos.y + fPosY_Delta,
                0.f });	// 이미지 위치

                m_pLeftHandComp->Set_Scale({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.x + fSizeX_Delta, 	// 이미지 크기
                                                (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.y + fSizeY_Delta,
                                                1.f });

                m_pLeftHandComp->Set_Rotation({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.x + fRotX_Delta, 	// 이미지 회전
                                                    (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.y + fRotY_Delta,
                                                    (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.z + fRotZ_Delta });

                // 텍스처 번호
                m_tLeftHand.fLeftFrame = _float((timeline)[KEYTYPE_LEFTHAND][iFrameIndex].texureframe);
            }
            else
            {
                m_pLeftHandComp->Set_Scale({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.x, 	// 이미지 크기
                    (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.y,
                    1.f });

                m_pLeftHandComp->Set_Rotation({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.x, 	// 이미지 회전
                        (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.y,
                        (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.z });


                m_pLeftHandComp->Set_Pos({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.x,
                                            (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.y + m_vIdlePos.y,
                                            0.f });	// 이미지 위치

                // 텍스처 번호
                m_tLeftHand.fLeftFrame = _float((timeline)[KEYTYPE_LEFTHAND][iFrameIndex].texureframe);
            }
        }
    }
}


void CPlayer::RightInterpolation() // 왼손, 오른손 선형 보간 함수 별개로 만들기
{
    if (!timeline[KEYTYPE_RIGHTHAND].empty()) // 해당 키타입이 비었는지 체크
    {
            if (m_tTime.fRightCurrentTime >= 0.f &&
                m_tTime.fRightCurrentTime <= timeline[KEYTYPE_RIGHTHAND].back().time)
            {
                // 프레임 0으로 초기화
                _uint iFrameIndex = 0U;

                // 사이즈의 끝에서부터 시작해서 찾기
                for (_uint j = _uint(timeline[KEYTYPE_RIGHTHAND].size()) - 1; j >= 0; j--)
                {
                    // 풀차지시 키프레임
                    if (timeline[KEYTYPE_RIGHTHAND][j].bFullChargeKeyframe)
                    {
                        m_tRightHand.iFullChargingIndex = j; // 풀차지 인덱스
                    }

                    // 쉴드시 키프레임
                    if (timeline[KEYTYPE_RIGHTHAND][j].bFullChargeKeyframe)
                    {
                        m_tRightHand.iShieldIndex = j; // 쉴드시 인덱스
                    }

                    if ((timeline)[KEYTYPE_RIGHTHAND][j].time <= m_tTime.fRightCurrentTime)
                    {
                        iFrameIndex = j;
                        break;
                    }
                }

                // Linear
                if (iFrameIndex + 1U < timeline[KEYTYPE_RIGHTHAND].size())
                {
                    // 키 프레임간 시간 변화율
                    fFrameTimeDelta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].time - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].time;

                    // 현재 키 프레임시간부터 현재 시간 변화율
                    fCurFrameTimeDelta = (m_tTime.fRightCurrentTime - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].time);

                    fSizeX_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vScale.x - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.x;
                    fSizeX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                    fSizeY_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vScale.y - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.y;
                    fSizeY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                    fRotX_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vRot.x - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.x;
                    fRotX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                    fRotY_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vRot.y - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.y;
                    fRotY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                    fRotZ_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vRot.z - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.z;
                    fRotZ_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                    fPosX_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vPos.x - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.x;
                    fPosX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
                    fPosY_Delta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].vPos.y - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.y;
                    fPosY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

                    m_pRightHandComp->Set_Pos({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.x + fPosX_Delta,
                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.y + m_vIdlePos.y + fPosY_Delta,
                    0.f });	// 이미지 위치

                    m_pRightHandComp->Set_Scale({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.x + fSizeX_Delta, 	// 이미지 크기
                                                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.y + fSizeY_Delta,
                                                    1.f });

                    m_pRightHandComp->Set_Rotation({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.x + fRotX_Delta, 	// 이미지 회전
                                                        (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.y + fRotY_Delta,
                                                        (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.z + fRotZ_Delta });

                    // 텍스처 번호
                    m_tRightHand.fRightFrame = _float((timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].texureframe);
                        
                }
                else
                {
                    m_pRightHandComp->Set_Scale({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.x, 	// 이미지 크기
                                                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.y,
                                                    1.f });

                    m_pRightHandComp->Set_Rotation({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.x, 	// 이미지 회전
                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.y,
                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.z });

                    m_pRightHandComp->Set_Pos({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.x,
                                                (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.y + m_vIdlePos.y,
                                                0.f });	// 이미지 위치

                    // 텍스처 번호
                    m_tRightHand.fRightFrame = _float((timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].texureframe);
                }
            }
    }
}

void CPlayer::LineEvent()
{
    CGameObject* pGameObject = nullptr;
    CAceGameObject* pAceObj = nullptr;
    CDynamicCamera* pCamera = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Camera", L"DynamicCamera"));

    if (pCamera)
    {
        _vec3 vTest = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vTest, &vTest);

        _vec3 vPos = m_pTransformComp->Get_Pos();
        vPos.y += 0.7f;

        /*auto listMonster = Engine::IntersectTests_Line_GetGameObject(0, m_pTransformComp->Get_Pos(),
            m_pTransformComp->Get_Pos() + vTest * 100.f,
            LAYER_MONSTER);*/
        auto listMonster = Engine::IntersectTests_Line_GetGameObject(0, vPos,
            vPos + m_pTransformComp->Get_Look() * 3.f,
            _ulong(-1) ^ LAYER_PLAYER);

        if (!listMonster.empty())
        {
            CGameObject* pDstObj = nullptr;
            _float fDist = FLT_MAX;

            for (auto iter = listMonster.begin(); iter != listMonster.end(); ++iter)
            {
                _vec3 vPos = (*iter).second.vContactPoint.Convert_DX9Vec3();
                _float fLength = D3DXVec3Length(&(m_pTransformComp->Get_Pos() - vPos));
                if (fLength < fDist)
                {
                    fDist = fLength;
                    pDstObj = (*iter).first;
                }
            }


            if (pDstObj)
            {
                cout << " 거리 : " << fDist << endl;
                CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDstObj);
                // 타겟 지정
                //CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

                //cout << " 체력 : " << pMonster->Get_MonsterHP().Cur << endl;

                pGameObject = pDstObj;
            }
            else
            {
                pGameObject = nullptr;
            }
        }

    }

    if (pGameObject != nullptr)
    pAceObj = dynamic_cast<CAceGameObject*>(pGameObject);

    if (pAceObj == nullptr)
        return;

    CAceWeapon* pWeapon = dynamic_cast<CAceWeapon*>(pAceObj);
    // Weapon 피킹 
    if (pWeapon != nullptr)
    {
        pWeapon->Set_Weapon_GearUp(true);
        m_eObjectName = pWeapon->Get_WeaponName();

        switch (m_eObjectName)
        {
        case CPlayer::OBJECT_NAME::NONE:
            Engine::Play_Sound(L"FallenAces", L"Crack.mp3", SOUND_PLAYER, m_tPlayerSound.m_fSoundVolume);
            pWeapon->Set_Dead();
            break;
        case CPlayer::OBJECT_NAME::GUN:
            Engine::Play_Sound(L"FallenAces", L"LooseSpin.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            Engine::Play_Sound(L"FallenAces", L"Nice (2).wav", SOUND_PLAYER, m_tPlayerSound.m_fSoundVolume);
            pWeapon->Set_Dead();
            break;
        case CPlayer::OBJECT_NAME::THOMPSON:
            Engine::Play_Sound(L"FallenAces", L"Reload.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            Engine::Play_Sound(L"FallenAces", L"Nice (2).wav", SOUND_PLAYER, m_tPlayerSound.m_fSoundVolume);
            pWeapon->Set_Dead();
            break;
        case CPlayer::OBJECT_NAME::STEELPIPE:
            Engine::Play_Sound(L"FallenAces", L"Nice (3).wav", SOUND_PLAYER, m_tPlayerSound.m_fSoundVolume);
            pWeapon->Set_Dead();
            break;
        case CPlayer::OBJECT_NAME::BEERBOTLE:
            Engine::Play_Sound(L"FallenAces", L"BottlePickup.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            Engine::Play_Sound(L"FallenAces", L"Nice (1).wav", SOUND_PLAYER, m_tPlayerSound.m_fSoundVolume);
            pWeapon->Set_Dead();
            break;
        case CPlayer::OBJECT_NAME::FRYINGPAN:
            Engine::Play_Sound(L"FallenAces", L"Nice (0).wav", SOUND_PLAYER, m_tPlayerSound.m_fSoundVolume);
            pWeapon->Set_Dead();
            break;
        }
    }

    CAceFood* pFood = dynamic_cast<CAceFood*>(pAceObj);
    // Food 피킹 
    if (pFood != nullptr)
    {
        pFood->Set_FOOD_EAT(true);

        m_gHp.Cur += pFood->Get_Hp();
        
        if (m_gHp.Cur >= 100.f)
            m_gHp.Cur = 100.f;

    }
    else
    { }
}

void CPlayer::RayAttack(CGameObject* _pDst, _float _fAttack, PLAYER_ATTACK_STATE _eAttackState)
{
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(_pDst);

    // Pow 생성
    Engine::Add_GameObject(L"GameLogic", CEffect_HitPow::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 0.2f, m_pTransformComp->Get_Pos().z, this));


    if (pAceObj == nullptr)
        return;

    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
    {
        // 타겟 지정
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(_pDst);

        // 공격받은 몬스터(타겟)가 있을 경우
        if (nullptr != pMonster)
        {
            // 이벤트 발생
            pMonster->OnCollisionEntered(this, nullptr);

            // 몬스터 체력 받아오기
            m_fMonsterHp = pMonster->Get_MonsterHP();

            // 몬스터 체력 계산
            m_fMonsterHp.Cur += _fAttack;

            if (m_fMonsterHp.Cur <= 0)
                m_fMonsterHp.Cur = 0.f;
            if (m_fMonsterHp.IsMax())
                m_fMonsterHp.Cur = m_fMonsterHp.Max;

            pMonster->Set_MonsterHP(m_fMonsterHp); // 계산이 끝난 몬스터의 체력으로 넣어줌
            pMonster->Set_Player_AttackState(_eAttackState); // 공격받은 유형을 넣어줌
        }
    }
}

//void Ray CPlayer::Event()
//{
//    return void Ray();
//}
