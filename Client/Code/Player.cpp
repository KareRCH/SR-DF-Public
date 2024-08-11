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

//�׽�Ʈ
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

#pragma region �� ���̵�

    Set_TeamID(ETEAM_PLAYER);

#pragma endregion
    

#pragma region ������

    Engine::Add_BlackBoard(L"Player", CBlackBoard_Player::Create());
    Engine::Add_BlackBoard(L"MonsterUnion", CBlackBoard_Monster::Create());

#pragma endregion

#pragma region �÷��̾� �浹ü
    //m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    //FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
    //pShape->fRadius = 0.5f;

    /*FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->fRadius = 5.f;*/
#pragma endregion

    //���� ����
    m_tPlayerSound.m_fTalkAge = 0.f;
    m_tPlayerSound.m_fTalkLife = 5.f; // �ݺ��� �ʿ��Ѿֵ��� ���� �̰ű������� 
    m_tPlayerSound.m_fTalkReapeat = 0.f;
    m_tPlayerSound.m_fSoundVolume = 0.6f;
    m_tPlayerSound.m_fSoundEffectVolume = 0.2f;
    m_tPlayerSound.m_bSoundOnce = false;
    m_tPlayerSound.m_bSoundCheck = false;

#pragma region �÷��̾� ũ�� �� ��ġ ���� (�ʱ� ��)
    // �÷��̾� ���� (�ʱⰪ)
    m_gHp.Max = 100.f;
    m_gHp.Cur = m_gHp.Max;
    m_fChage.Max = 2.f; // ������ �Ϸ� �� �ð�
    m_tTime.fChargeStartTime = 1.f; // ������ ������ �ð�
    m_bGunLight = false;
    bRightPunch = true;
    bLeftPunch = false;
    m_PlayerLighter = false;

    m_tLeftHand.bPickUpState = true;
    m_tRightHand.bPickUpState = true;

    //m_pMeshComp->BoxMesh_Ready(m_pGraphicDev, 10.f, 10.f, 5.f);

    // �÷��̾� ��� �ʱ�ȭ
    m_pTransformComp->Set_Pos({ 10.f, 10.f, 10.f }); // Test Stage
    //m_pTransformComp->Set_Pos({ 20.f, 5.f, -9.5f }); // Jump Map
    m_pTransformComp->Readjust_Transform();

    m_pLeftHandComp->Set_Pos({ -300.f, -363.f, 0.f });	                        // �̹��� ��ġ
    m_pLeftHandComp->Set_Scale({ 1495.f, 1588.f, 1.f });						// �̹��� ũ��

    m_pRightHandComp->Set_Pos({ 0.f, 0.f, 0.f });	                            // �̹��� ��ġ
    m_pRightHandComp->Set_Scale({ 1495.f, 1588.f, 1.f });						// �̹��� ũ��
#pragma endregion

#pragma region �ʱ� ���� ���� (���� ����)  
    m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);           // �÷��̾� 
    m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);       // �޼�  
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);;    // ������
#pragma endregion


#pragma region �÷��̾��� ���� �߰�
    m_tPlayer_State.Add_Func(STATE_PLAYER::IDLE, &ThisClass::Idle);         // �⺻ ����
    m_tPlayer_State.Add_Func(STATE_PLAYER::SITDOWN, &ThisClass::Down);      // ���� ����
    m_tPlayer_State.Add_Func(STATE_PLAYER::DIE, &ThisClass::Die);           // ���� ����
#pragma endregion

#pragma region �÷��̾��� �ൿ �߰�
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::IDLE, &ThisClass::Action_Idle);              // �⺻ (����)
    //m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::MOVE, &ThisClass::Action_Move);              // �̵�
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::RUN, &ThisClass::Action_Run);                       // �޸���
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::CHARGING, &ThisClass::Action_Charging);      // ��¡
    m_tPlayer_Action.Add_Func(STATE_PLAYER_ACTION::THROW_AWAY, &ThisClass::Action_ThrowAway);   // ������
#pragma endregion

#pragma region �÷��̾��� �޼� ���� �߰�
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::NONE, &ThisClass::Left_None);                // �޼� ����
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::HAND, &ThisClass::Left_Hand);                // �޼� �ָ�
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_RIHGTER, &ThisClass::Left_RunRighter);   // �޼� ������ ��� �ٰ�����
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::OPEN_HAND, &ThisClass::Left_OpenHand);       // ������ ������Ʈ
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RUN_HAND, &ThisClass::Left_RunHand);         // �ٰ�����
    m_tLeftHand_State.Add_Func(STATE_LEFTHAND::RIGHTER, &ThisClass::Left_Righter);          // ������
#pragma endregion


#pragma region �÷��̾��� ������ ���� �߰�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::NONE, &ThisClass::Right_None);             // ������ ����
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::HAND, &ThisClass::Right_Hand);             // ������ �ָ�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::GUN, &ThisClass::Right_Gun);               // ����
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::RUN_HAND, &ThisClass::Right_RunHand);      // �޸���
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::THOMPSON, &ThisClass::Right_Thompson);     // �轼 �����
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::STEELPIPE, &ThisClass::Right_Steelpipe);   // ��������
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::BEERBOTLE, &ThisClass::Right_BeerBotle);   // ���ֺ�
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::FRYINGPAN, &ThisClass::Right_FryingPan);   // ��������
    m_tRightHand_State.Add_Func(STATE_RIGHTHAND::KICK, &ThisClass::Right_Kick);             // ������
#pragma endregion

#pragma region �׼� Ű
    for (size_t i = 0; i < static_cast<size_t>(EACTION_KEY::SIZE); i++)
        m_tActionKey.Add_Action(static_cast<EACTION_KEY>(i));
#pragma endregion

    // Tset (������Ʈ �޾ƿ��°�)
    m_eObjectType = OBJECT_TYPE::NONE; // �ʱ���� : ��� �ָ�
    m_eObjectName = OBJECT_NAME::NONE; // �ʱ���� : ����

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

    // �÷��̾� ��� �ʱ�ȭ
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

    // ���콺 ������
    Mouse_Move();
    // ���콺 �Է�
    Attack_Input(fTimeDelta);
    // Ű���� �Է�
    Keyboard_Input(fTimeDelta);

    // ���� ������Ʈ(üũ)
    State_Update(fTimeDelta);

    PlayerJump(fTimeDelta);

    // ���� Ÿ��
    if (m_pTransformComp->Get_Pos().y < 1.7f && m_vSpeed.y < 0.f)
    {
        Height_On_Terrain();
        m_IsOnGround = true;
    }
    else
        m_IsOnGround = false;

    // �뽬
    Dash(fTimeDelta);

    // ������ ����
    FrameManage(fTimeDelta);

    // ���� ������Ʈ �ڵ�
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹ü �̵�

    //�÷��̾� ����׿� �ڵ� 
    //swprintf_s(debugString, L"Player - ���� Ȯ�� m_gHp = %f\n", m_gHp.Cur);
    //OutputDebugStringW(debugString);


    //�÷��̾� �ڷ� �ݶ� �����Ǵ� �ڵ�
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
    //    if ((*iter) != this) // �ڱ��ڽ��� ������ �ʴ� �����ڵ� ���� 
    //        (*iter)->Set_Dead();
    //}
        // ���� �ǰ� �׽�Ʈ�� 


    m_tActionKey.Update();

    // ������ �����ϱ�
    Update_BlackBoard();

    // ������ �����ޱ�
    Update_InternalData();

    // ���� �׷� ����, ������� : ���� �׽�Ʈ
    Engine::Add_RenderGroup(RENDER_UI, this);

    // ���� �׷� ���
    //RENDER_PRIORITY, RENDER_ALPHATEST, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END

    return 0;
}

void CPlayer::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
#pragma region �ɼ�
    // ��� ����
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    //m_pMeshComp->BoxMesh_Col();
    // ���� ���� �ɼ�
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#pragma endregion

#pragma region �޼�
    // �޼� ��ġ ����
    m_pLeftHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pLeftHandComp->Get_Transform());

    // �޼� ��� ����
    if (m_tLeftHand.bLeftHandOn)
    {
        // �޼� �ؽ�ó ���
        m_pLeftHandComp->Render_Texture((_ulong)m_tLeftHand.fLeftFrame, true);    // �޼� �ؽ�ó ���
        m_pBufferComp->Render_Buffer();                                 // �޼� ���� 
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

#pragma region ������
    // ������ ��ġ ����
    m_pRightHandComp->Readjust_Transform();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pRightHandComp->Get_Transform());

    // ������ ��� ����
    if (bRightHandOn)
    {
        m_pRightHandComp->Render_Texture(_uint(m_tRightHand.fRightFrame), true);    // ������ �ؽ�ó ���
        m_pBufferComp->Render_Buffer();                             // ������ ����
    }
#pragma endregion

#pragma region �ɼ� ���� 
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#pragma endregion
}

HRESULT CPlayer::Add_Component()
{
#pragma region ������Ʈ
    // ���� ������Ʈ
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    // ��� ������Ʈ
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    // ����Ÿ�� ������Ʈ
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    // �ݶ��̴� ������Ʈ
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderCapsuleComp"), E_FAIL);
    // �޼� ������Ʈ
    NULL_CHECK_RETURN(m_pLeftHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureLeftHandTest", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // ������ ������Ʈ
    NULL_CHECK_RETURN(m_pRightHandComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureRightHandTest", L"Proto_PlayerRightTextureComp"), E_FAIL);
#pragma endregion
    //NULL_CHECK_RETURN(m_pMeshComp = Set_DefaultComponent_FromProto<CMeshColComp>(ID_STATIC, L"Com_Mesh", L"Proto_MeshColBufferComp"), E_FAIL);

    // �ؽ�ó �Ŵ���
    // �޼�
    m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
    // ������
    m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

    // ���� ���� ���
    m_pColliderComp->EnterToPhysics(0);
    // �浹 �Լ� ����
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);
    // �浹 ���̾�, ����ũ ����
    m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER);
    m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER |
                                      LAYER_BOSS_SKILL | LAYER_MONSTER_ATTACK |
                                      LAYER_ITEM | LAYER_WALL);
    m_pColliderComp->Set_Scale(_vec3(1.f, 3.f, 1.f));
    m_pColliderComp->Readjust_Transform();
    // �÷��̾ shift�� �뽬�� �ϰų� ������ �������� ���Ϳ� �浹�� ����. 

    return S_OK;
}

bool CPlayer::Keyboard_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);

    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = m_tCamera.vAt;

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

#pragma region Ű�Է�
    // ����
    if (Engine::IsKey_Pressing(DIK_W))
    {
        // �÷��̾� �ൿ : �ȱ�
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);
        
        if (!m_tRightHand.bPickUpState)
        {
            // �ٱ�
            if (Engine::IsKey_Pressing(DIK_LSHIFT))
            {
                // ���� �ӵ� Up
                m_tPlayer.fStraightSpeed = 10.f;
                // �÷��̾� �ൿ : �޸���
                m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::RUN);
                m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
                m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);

                m_tRightHand.bRightFrameOn = true;
                m_tLeftHand.bLeftFrameOn = true;
            }
        }
        // �ٱ�
        if (Engine::IsKey_Released(DIK_LSHIFT))
        {
            // �÷��̾� ��� : �ʱ�ȭ
            m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::NONE);
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);

            // ���� �ӵ� ����
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



    // ����
    if (Engine::IsKey_Pressing(DIK_S))
    {
        // �÷��̾� �ൿ : �ȱ�
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);

        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -m_tPlayer.fSpeed);

        // S + Shift �� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_tDash.fDownDash = 0.f;    // �뽬 ����(ī�޶�) ����
            m_tDash.fDash = 20.f;       // �뽬 �� ����
            bDashOn = true;     // �뽬 On/Off
            m_eDashDir = DOWN;  // �뽬 ����
        }
    }

    // ������
    if (Engine::IsKey_Pressing(DIK_D))
    {
        // �÷��̾� �ൿ : �ȱ�
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);

        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&vRight, fTimeDelta, m_tPlayer.fSpeed);

        // fSpeed = 5.f , fDash = 20.f
        // D + Shift ���� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_tDash.fDownDash = 0.f;    // �뽬 ����(ī�޶�) ����
            m_tDash.fDash = 20.f;       // �뽬 �� ����
            bDashOn = true;     // �뽬 On/Off
            m_eDashDir = RIGHT; // �뽬 ����
        }
    }

    // ����
    if (Engine::IsKey_Pressing(DIK_A))
    {
        // �÷��̾� �ൿ : �ȱ�
        //m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::MOVE);

        _vec3 vRight;
        m_pTransformComp->Get_Info(INFO_RIGHT, &vRight);
        D3DXVec3Normalize(&vRight, &vRight);
        m_pTransformComp->Move_Pos(&-vRight, fTimeDelta, m_tPlayer.fSpeed);

        // A + Shift ���� �뽬
        if (Engine::IsKey_Pressed(DIK_LSHIFT))
        {
            m_tDash.fDownDash = 0.f;    // �뽬 ����(ī�޶�) ����
            m_tDash.fDash = 20.f;       // �뽬 �� ����
            bDashOn = true;     // �뽬 On/Off
            m_eDashDir = LEFT;  // �뽬 ����
        }
    }

    // ��
    if (Engine::IsKey_Pressed(DIK_1))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_HAND;
        m_eObjectName = OBJECT_NAME::NONE;
    }

    // ����
    if (Engine::IsKey_Pressed(DIK_2))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::GUN;
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
    }

    // �轼 �����
    if (Engine::IsKey_Pressed(DIK_3))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::TWO_OBJECT;
        m_eObjectName = OBJECT_NAME::THOMPSON;
    }

    // ��������
    if (Engine::IsKey_Pressed(DIK_4))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::STEELPIPE;
    }

    // ���ֺ�
    if (Engine::IsKey_Pressed(DIK_5))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::BEERBOTLE;
    }

    // ��������
    if (Engine::IsKey_Pressed(DIK_6))
    {
        //Test
        m_eObjectType = OBJECT_TYPE::RIGHT_OBJECT;
        m_eObjectName = OBJECT_NAME::FRYINGPAN;
    }

    // ����
    if (Engine::IsKey_Pressed(DIK_SPACE))// && m_IsOnGround)
    {
        m_vSpeed.y = 11.f;
    }

    // ������
    //if (Engine::IsKey_Pressed(DIK_Q))
    //{
    //    m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK);
    //}

    // ������
    if (Engine::IsKey_Pressed(DIK_V) && m_tPlayer_Action.Get_State() != STATE_PLAYER_ACTION::RUN)
    {
        m_tLeftHand.bLeftFrameOn = true;

        if (!bRighter)  // �����Ͱ� �������� ���
        {
            bRighter = true;        // ������ ���ֱ�
            Engine::Play_Sound(L"FallenAces", L"Open_Zippo.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER); // �޼� ���� �����ͷ�
            m_tLeftHand.fLeftFrame = 0.f;
            m_tTime.fLeftCurrentTime = 0.f;
        }
        else // �����Ͱ� �������� ���
        {
            Engine::Play_Sound(L"FallenAces", L"Close_Zippo.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            bBackRighter = true; // ������ �ǵ�����On
            m_tLeftHand.fLeftFrame = 0.f;
            m_tTime.fLeftCurrentTime = 0.f;
        }
    }

    // ������Ʈ ������ (������Ʈ�� ���� ��츸 ���డ��)
    if (Engine::IsKey_Pressed(DIK_F) && m_eObjectName != OBJECT_NAME::NONE)
    {
        // �÷��̾� ���� ������Ʈ ������ ��
        Engine::Play_Sound(L"FallenAces", L"Drop_Item.wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::THROW_AWAY);
    }

    // �ɱ� (������ ��)
    if (Engine::IsKey_Pressing(DIK_C))
    {
        if (m_tPlayer_State.Get_State() != STATE_PLAYER::SITDOWN) // �̹� �������°� �ƴҶ� 
        {        
            // �÷��̾� ���� �ɱ�
            m_tPlayer_State.Set_State(STATE_PLAYER::SITDOWN);

            _vec3 vSitDown = { 0.f, 5.f, 0.f };

            // �뽬 ����
            m_pTransformComp->Set_MovePos(fTimeDelta, vSitDown);
            //m_pTransformComp->Set_PosY(-10.f);
        }
        else //���������϶� -> �Ͼ���� 
            m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
    }

    // Test
    if (Engine::IsKey_Pressed(DIK_E))
    {
        LineEvent();
    }


    // ���� ��
    if (Engine::IsKey_Released(DIK_C))
    {
        // �÷��̾� ���� �ǵ���
        m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
    }

    // ���� ������ Ȯ��
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
    
    // ������Ʈ ��ȣ�ۿ� // �ҿ� �߰� 
    /*if (Engine::IsKey_Pressed(DIK_E) && Picking_On_Object_Local())
    {
        int a = 0;
    }*/
#pragma endregion

    // �÷��̾� �⺻ �ӵ�
    m_tPlayer.fSpeed = 5.f;

    return false;
}

void CPlayer::FrameManage(const _float& fTimeDelta)
{
#pragma region �޼� ������On
if (!timeline[KEYTYPE_LEFTHAND].empty())
{
    // �޼� ���� ����
    LeftInterpolation();

    // �޼� �������� ������ ����
    if (m_tLeftHand.bLeftFrameOn)
    {

        // ������ �ǵ����Ⱑ �������� ���
        if (bBackRighter)
        {
            LeftLoadAnimationFromFile("BackRighter"); // �ǵ����� �ִϸ��̼�
        }
        // ���� �ð�
        m_tTime.fLeftCurrentTime += m_tTime.fLeftChangeTime * fTimeDelta;

        // ���� �������� �ִ� �����ӿ� ������ ���
        if (m_tTime.fLeftCurrentTime >= m_tTime.fLeftMaxTime)
        {
            // ������ ������� ���
            if (m_tLeftHand.bPickUpState)
            {
                m_tLeftHand.bPickUpState = false;
            }
            else // ������ ����� �ƴ� ���
            {
                // �޼� �����Ӱ� �ð� �ʱ�ȭ
                m_tLeftHand.fLeftFrame = 0.f;
                m_tTime.fLeftCurrentTime = 0.f;
                m_tLeftHand.bLeftFrameOn = false;

                // ����� �ָ��ε� �������� �ִ� �����ӿ� ������ ���
                if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                    m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
                {
                    if (m_bAttack && bLeftPunch)
                    {
                        // �޼� �����Ӱ� �ð� �ʱ�ȭ
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

            // ���� �ִ��������ε� �����Ͱ� �������� ���
            if (bRighter)
            {
                // (���� ������) �����͸� �����ִ� �̹���(������)�� ����
                m_tLeftHand.fLeftFrame = _float(timeline[KEYTYPE_LEFTHAND].back().texureframe);
                m_PlayerLighter = true;
                //m_tLeftHand.bLeftFrameOn = false;
            }
            else // �����Ͱ� ���������� ���
            {
                // ���� ������ �ʱ�ȭ
                m_tLeftHand.fLeftFrame = 0.f;
                m_tPlayer_State.Set_State(STATE_PLAYER::IDLE);
            }
            // ������ �ǵ����Ⱑ �������� ���
            if (bBackRighter)
            {
                bRighter = false;
                bBackRighter = false;
                m_PlayerLighter = false;
                m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE); // �޼� ���� �ʱ�ȭ
            }
        }
    }
}
#pragma endregion

#pragma region ������ ������On (�޼հ� ����)
if (!timeline[KEYTYPE_RIGHTHAND].empty())
{
    // ������ ���� ����
    RightInterpolation();

    // ������ �������� ������ ����
    if (m_tRightHand.bRightFrameOn)
    {
        // ���� �ð�
        m_tTime.fRightCurrentTime += m_tTime.fRightChangeTime * fTimeDelta;

        // ��¡ ������ ������, ���� �������� Ǯ ��¡ �����ӿ� �������� ���
        if (bChargeAttack &&
            m_tTime.fRightCurrentTime >= timeline[KEYTYPE_RIGHTHAND][m_tRightHand.iFullChargingIndex].time)
        {
            // ���� �ð��� �ؽ�ó �̹����� Ǯ ��¡�� ����
            m_tRightHand.fRightFrame = _float(timeline[KEYTYPE_RIGHTHAND][m_tRightHand.iFullChargingIndex].texureframe);
            m_tTime.fRightCurrentTime = timeline[KEYTYPE_RIGHTHAND][m_tRightHand.iFullChargingIndex].time;
        }

        // �÷��̾� ������ �� �������� �� ������ ���
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::KICK &&
            m_tTime.fRightCurrentTime >= m_tTime.fRightMaxTime)
        {
            m_tPlayer_State.Set_State(STATE_PLAYER::NONE); // �÷��̾� ���� �ʱ�ȭ
            Two_Hand(); // �� �ָ����� ���ư��� (���߿� �������·� ���ư��� �ؾ���)
        }

        // ������ �������� �ִ� �����ӿ� �������� ���
        if (m_tTime.fRightCurrentTime >= m_tTime.fRightMaxTime)
        {
            // ���� �ִ��������ε� ������ ���� ���
            if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
            {
                // (���� ������) �������� Throw �̹����� ����
                m_tRightHand.fRightFrame = _float(timeline[KEYTYPE_RIGHTHAND].back().texureframe);

                // �ð����� �ؽ�ó ���� �ð��� ä���ش�.
                m_tTime.fRightCurrentTime += m_tTime.fRightChangeTime * fTimeDelta;
            }
            else // ������ ���� �ƴ� ���
            {
                // �÷��̾� ��¡ ���� ���� ���
                if (m_tRightHand.bChargingAttackOn)
                {
                    m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::IDLE); // �÷��̾� �ൿ : �ʱ�ȭ

                    m_bAttack = false;
                    m_tRightHand.bBasicAttackOn = true; // �⺻ �������� �ǵ�����
                    m_tRightHand.bChargingAttackOn = false;
                    m_tRightHand.bRightAttacColOn = true;
                }
                else
                {
                    // ����� �ָ� ���� �ϰ��
                    if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                        m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
                    {
                        // ������ �������� �� ������ ���
                        if (m_bAttack && bRightPunch)
                        {
                            // ������ �����Ӱ� �ð� �ʱ�ȭ
                            m_tRightHand.fRightFrame = 0.f;
                            m_tTime.fRightCurrentTime = 0.f;

                            m_bAttack = false;
                            //m_tRightHand.bRightAttacColOn = true;

                            bLeftPunch = true;
                            bRightPunch = false;
                        }
                    }
                }
                // ������ ������� ���
                if (m_tRightHand.bPickUpState)
                {
                    m_tRightHand.bPickUpState = false;
                }
                else // ������ ����� �ƴ� ���
                {
                    // ������ �����Ӱ� �ð� �ʱ�ȭ
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
                cout << " �Ÿ� : " << fDist << endl;
                CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDstObj);
                // Ÿ�� ����
                //CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

                //cout << " ü�� : " << pMonster->Get_MonsterHP().Cur << endl;

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
    // �浹���϶�
   // OutputDebugString(L"�÷��̾�� �浹��\n");
    
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
        if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // �������
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

    //�÷��̾� Hostile���� =  Monster , Boss
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // �������
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

                // �浹�Ѱ� ������ ��ų�϶�  �����ϴ°� 
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

            // �浹�Ѱ� �����϶� �����ϴ°�
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


    //// ó�� �浹������
    //CBrown* pBrown = dynamic_cast<CBrown*>(pDst->Get_Owner());
    //if (FAILED(pBrown))
    //{
    //    CGray* pBrown = dynamic_cast<CGray*>(pDst->Get_Owner());
    //    OutputDebugString(L"�÷��̾� - Gray  �浹��\n");
    //}
    //else
    //{
    //    OutputDebugString(L"�÷��̾� - Brown  �浹��\n");
    //}
}

void CPlayer::OnCollisionExited(CGameObject* pDst)
{
    m_bHitState = false;
    // �浹���� ������
   // OutputDebugString(L"�÷��̾�� �浹�Ϸ�\n");
}

void CPlayer::CrossHairState()
{
    //������ ���ε�� ���� ��� ������� 


}

// ������ ���� ������Ʈ (���� �÷��ֱ�)
void CPlayer::Update_BlackBoard()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // ���� ����
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // ���⼭���� �������� ������ ������Ʈ �Ѵ�.
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
        

    //pBlackBoard->Get_PlayerPickUpState() = m_tRightHand.; //��¡����
    //pBlackBoard->Get_PlayerEquipGunState() = m_tRightHand.; //��¡����


}

// ������ ���� �����ޱ� (���� �����ޱ�)
void CPlayer::Update_InternalData()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Camera.Get_BlackBoard())
    {
        m_wpBlackBoard_Camera.Set_BlackBoard(Engine::Get_BlackBoard(L"Camera"));
        // ���� ����
        if (!m_wpBlackBoard_Camera.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Camera* pBlackBoard = m_wpBlackBoard_Camera.Get_BlackBoard();

    // ���⼭���� �������� ������ ���´�.
    m_tCamera.bOne = pBlackBoard->Get_OneSwitch();
    m_tCamera.bThree = pBlackBoard->Get_ThreeSwitch();
    m_tCamera.vEye = pBlackBoard->Get_Eye();
    m_tCamera.vAt = pBlackBoard->Get_At();
}

bool CPlayer::Attack_Input(const _float& fTimeDelta)
{
    //������
   //if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
   //{
   //    m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK);
   //}

#pragma region ���콺

    // ���콺 ��Ŭ�� (������ ���� ��)
    if (Engine::IsMouse_Pressing(DIM_LB))
    {
        if (!m_bAttack) // ��¡�� ������ ������ ����
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
                    // ������ ������ �ð�
                    if ((m_fChage.Update(1.f * fTimeDelta, m_tTime.fChargeStartTime)))
                    {
                        m_tRightHand.bRightFrameOn = true;

                        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::CHARGING);
                    }
                    break;
                }
                case STATE_RIGHTHAND::FRYINGPAN:
                {
                    // ������ ������ �ð�
                    if ((m_fChage.Update(1.f * fTimeDelta, m_tTime.fChargeStartTime)))
                    {
                        m_tRightHand.bRightFrameOn = true;

                        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::CHARGING);
                    }
                    break;
                }
                case STATE_RIGHTHAND::HAND:
                {
                    // ������ ������ �ð�
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

    // ��¡�� ��������, ������ ���� �ƴ� ���
    if (!m_tRightHand.bPickUpState &&
        m_tPlayer_Action.Get_State() != STATE_PLAYER_ACTION::CHARGING)
    {
        m_tPlayer_Action.Set_State(STATE_PLAYER_ACTION::ATTACK);

        if (Engine::IsMouse_Pressed(DIM_LB)) // ������ ��
        {
            m_bAttack = true; // ���� On

            // ���� ä�� ������ ������ ��
            if (m_tPlayer_State.Get_State() == STATE_PLAYER::SITDOWN)
            {
                m_eAttackState = PSITDONW_ATTACK; // �ɾƼ� ����
            }
            else if (false) // �� �� �ٸ������� ���� �߰�
            {

            }
            else // Ư�� ���°� ���� ��� �븻���� ����
            {
                m_eAttackState = PNOMAL_ATTACK;
            }
                
            // ����� �ָ� ���� �ϰ��
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
            else // ������ ����
            {
                // �÷��̾� ���� : ����, �÷��̾� ������ ������ On
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

    // ���콺 ��Ŭ�� (�� ��)
    if (Engine::IsMouse_Released(DIM_LB))
    {
        m_fChage.Cur = 0.f;
    }

    // ���콺 ��Ŭ��
    if (Engine::IsMouse_Pressing(DIM_RB))
    {
        // ���� ���� �϶�
        if (timeline[KEYTYPE_RIGHTHAND].front().bShieldPossible)
        {
            // ���� �������� 0�� ��
            if (m_tRightHand.fRightFrame == 0)
            {
                // ����On
                bShieldOn = true;

                // ����� �ָ� ���� �ϰ��
                if (m_tLeftHand_State.Get_State() == STATE_LEFTHAND::HAND &&
                    m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
                {

                }
            }
        }
    }
    else
    {
        // ����Off
        bShieldOn = false;
    }
#pragma endregion

    return false;
}

// ���콺 ������
void CPlayer::Mouse_Move()
{
#pragma region 1��Ī
    if (m_tCamera.bOne)
    {
        //m_pTransformComp->Rotate(ROT_X, m_pCamera->Get_At());

        //���콺�� �÷��̾� ȸ��
        //��, ��
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
        {
            m_pTransformComp->Rotate(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
        }
        // ��, ��
        if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
        {
            m_pTransformComp->Rotate(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
        }
    }
#pragma endregion

#pragma region 3��Ī ���ʹϾ�
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

        // ī�޶� �ٶ󺸴� At
        //m_vCameraAt = m_pCamera->Get_At();

        // �÷��̾ �ٶ󺸴� Look
        //m_pTransformComp->Get_Info(INFO_LOOK, &m_vPlayerLook);

        // ī�޶�� �÷��̾ �ٶ󺸴� ���̸� �Ȱ��� �����ش�.
        CameraToPlayer.y = 0.f;
        //m_vPlayerLook.y = 0.f;

        // ī�޶�� �÷��̾� ���͸� ����ȭ
        D3DXVec3Normalize(&CameraToPlayer, &CameraToPlayer);
        D3DXVec3Normalize(&vLook, &vLook);

        float fAngle;


        if (CameraToPlayer.x > 0)
        {
            // ������ �̿��� �� ���� ���ΰ��� �����ش�.
            fDot = D3DXVec3Dot(&CameraToPlayer, &vLook);
            // �� ���� ������ ���� ��� (����)
            fAngle = acosf(fDot);
        }
        else
        {
            //vLook = vLook * -1;
            // ������ �̿��� �� ���� ���ΰ��� �����ش�.
            fDot = D3DXVec3Dot(&CameraToPlayer, &(vLook * -1));
            // �� ���� ������ ���� ��� (����)
            fAngle = acosf(fDot);

            fAngle = fAngle + 3.141592f;
        }

        m_pTransformComp->Set_ScaleY(fAngle);
    }
#pragma endregion

#pragma region 1��Ī ���ʹϾ� ȸ�� (���)
    //dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
    //dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

    //D3DXQUATERNION quatPitch, quatYaw, quatRoll;

    //// �÷��̾��� Up ���͸� �����´�.
    //m_pTransformComp->Get_Info(INFO_UP, &m_vUp);

    //// �÷��̾��� Look ���͸� �����´�.
    //m_pTransformComp->Get_Info(INFO_LOOK, &m_vLook);

    //// ������ �÷��̾��� Up, Look ���ͷ� Right���� ���ϱ�
    //D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);

    //// ���� ȸ�� (Y�� ȸ��)
    //if (dwMouseMoveX != 0)
    //{
    //    // ���콺 X �̵��� ���� Y�� ������ ȸ���ϴ� ���ʹϾ� ���
    //    D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));

    //    // ���� ���ʹϾ� ���ο� ȸ���� ���Ͽ� ������Ų��.
    //    m_quaternion *= quatYaw;

    //    // ������ ȸ�� ���ʹϾ� ����ȭ
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// ���� ȸ�� (X�� ȸ��)
    //if (dwMouseMoveY != 0)
    //{
    //    // ���콺 X �̵��� ���� Y�� ������ ȸ���ϴ� ���ʹϾ� ���
    //    D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));

    //    // ���� ���ʹϾ� ���ο� ȸ���� ���Ͽ� ������Ų��.
    //    m_quaternion *= quatPitch;

    //    // ������ ȸ�� ���ʹϾ� ����ȭ
    //    D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
    //}

    //// ���ʹϾ��� ����Ͽ� ��ȯ ����� ����ϱ�
    //D3DXMATRIX matRotation;
    //D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion);

    //// ����� �÷��̾��� ��ȯ�� �����ϱ�.
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

    float fDashHeight = 4.0f; // �뽬 ���� ���� ���� (������)

    // �뽬 ����
    if (bDashOn)
    {
        // �뽬 ���� �ִ�ġ
        if (m_tDash.fDownDash >= 12.f)
        {
            bDashChange = true; // ���� ����
        }

        if (bDashChange)
        {
            m_tDash.fDownDash -= fDashHeight * 0.2f; // �ٿ� ��ġ ����
        }
        else
        {
            m_tDash.fDownDash += fDashHeight * 0.2f; // �ٿ� ��ġ ����
        }

        float fTime = 1.f * fTimeDelta;

        // �뽬 ����
        m_pTransformComp->Set_MovePos(fTime, vSpeed);

        // �뽬 �ӵ��� ���ǵ庸�� ���� ��츸 �۵�
        if (m_tDash.fDash > m_tPlayer.fSpeed)
        {
            // �뽬 ���� �� �̵�
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
            m_tDash.fDash--; // �뽬 �ӵ� ����
        }
        else // �뽬 �ӵ��� ���ǵ�� ������
        {

        }

        if (m_tDash.fDownDash <= 0.f)
        {
            bDashChange = false;
            bDashOn = false; // �뽬 ����
        }
    }

}

#pragma region ��� �ָ� (�⺻ ����)
void CPlayer::Two_Hand()
{
    // �ִ� ������ ����
    //m_fLeftMaxFrame = 2.f;
    //m_fRightMaxFrame = 2.f;

    m_eObjectName = OBJECT_NAME::NONE; // ������Ʈ ����

    // ������ �ָ�
    m_eRIGHTState = STATE_RIGHTHAND::HAND;
    m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);

    // �����͸� ������ ���
    if (!bRighter)
    {
        // �޼� �ָ�
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
    }

}
#pragma endregion

#pragma region ��� ������Ʈ
void CPlayer::Two_Object()
{
    // �����Ͱ� �����ִ� ���·� ��� ������Ʈ�� ������ ���
    if (bRighter)
    {
        // �޼� ���� ������ �ʱ�ȭ
        m_tLeftHand.fLeftFrame = 0.f;
        bRighter = false;       // ������ ����
    }

    switch (m_eObjectName)
    {
        // �轼 �����
    case CPlayer::OBJECT_NAME::THOMPSON:
    {
        // ������ �轼 ����� (���)
        m_eRIGHTState = STATE_RIGHTHAND::THOMPSON;
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::THOMPSON);
        // �޼� ����
        m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
        break;
    }
    }
}
#pragma endregion

#pragma region �Ѽ� ������Ʈ
void CPlayer::Right_Object()
{
    // ������
    switch (m_eObjectName)
    {
        // ����
        case CPlayer::OBJECT_NAME::GUN:
        {
            // ������ ���� (�Ѽ�)
            m_eRIGHTState = STATE_RIGHTHAND::GUN;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::GUN);
            // �޼� ����
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
            break;
        }
        // ��������
        case CPlayer::OBJECT_NAME::STEELPIPE:
        {
            // ������ ��������
            m_eRIGHTState = STATE_RIGHTHAND::STEELPIPE;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::STEELPIPE);
            // �޼� ���� �ڵ�
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        // ���ֺ�
        case CPlayer::OBJECT_NAME::BEERBOTLE:
        {
            // ������ ���ֺ�
            m_eRIGHTState = STATE_RIGHTHAND::BEERBOTLE;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::BEERBOTLE);
            // �޼� ���� �ڵ�
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        // ��������
        case CPlayer::OBJECT_NAME::FRYINGPAN:
        {
            // ������ ��������
            m_eRIGHTState = STATE_RIGHTHAND::FRYINGPAN;
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::FRYINGPAN);
            // �޼� ���� �ڵ�
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
    }

    // �޼�
    if (!bRighter) // �����͸� ������ ���
    {
        switch (m_tLeftHand_State.Get_State()) // ���� �޼� ����
        {
        case CPlayer::STATE_LEFTHAND::NONE: // �޼� ����
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE);
            break;
        }
        case CPlayer::STATE_LEFTHAND::HAND: // �޼� �ָ�
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
            break;
        }
        case CPlayer::STATE_LEFTHAND::OPEN_HAND: // �޼� ���� �ڵ�
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::OPEN_HAND);
            break;
        }
        }
    }
}
#pragma endregion

// �� ���� Ȯ��
void CPlayer::Hand_Check()
{
    // �޼� ������ ��� On
    m_tLeftHand.bLeftHandOn = true;
    bRightHandOn = true;
    
    // �÷��̾��� ���� ���� ���°� �ƴҰ��
    if (m_eRightState_Old != m_tRightHand_State.Get_State())
    {
        m_tTime.fLeftCurrentTime = 0.f;
        m_tTime.fRightCurrentTime = 0.f;

        m_tTime.fLeftChangeTime = 1.f;         // ������ �ӵ� ����
        m_tTime.fRightChangeTime = 1.f;         // ������ �ӵ� ����
        bRightGetAnimation = true; // ���ο� �ִϸ��̼� �ҷ����� On
        
        m_tRightHand.bRightFrameOn = true;
        m_tRightHand.bPickUpState = true;
        m_tLeftHand.bPickUpState = true;

        // ���ο� ���°� ������ ���
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::GUN)
        {

        }

        // ���ο� ���°� �ָ��� ���
        if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::HAND)
        {
            m_tLeftHand.bLeftFrameOn = true;
            bLeftGetAnimation = true;
        }

        // ���ο� ���°� �ٴ����� ���
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

    // �÷��̾��� ���� ���¸� ����
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

    // �÷��̾ �ȶٰ��ִ� ���
    if (m_tPlayer_Action.Get_State() != STATE_PLAYER_ACTION::RUN)
    {
        // �÷��̾ ������Ʈ�� ���������� ���
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
        {
            // ������Ʈ �ʱ�ȭ (����)
            m_eObjectType = OBJECT_TYPE::NONE;
            m_eObjectName = OBJECT_NAME::NONE;
        }

        if (bRighter) // �÷��̾ �����͸� Ų ���
        {
            // �޼� ������
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RIGHTER);
            bLeftGetAnimation = true;
        }

#pragma region ������Ʈ Ÿ�� üũ
        if (m_tRightHand_State.Get_State() != STATE_RIGHTHAND::KICK) // �÷��̾ �����⸦ ���� ���
        {
            // ������Ʈ�� NONE�� ���
            if (m_eObjectType == OBJECT_TYPE::NONE ||
                m_eObjectName == OBJECT_NAME::NONE)
            {
                m_eObjectType = OBJECT_TYPE::TWO_HAND; // �Ǽ�
            }

            switch (m_eObjectType)
            {
                case CPlayer::OBJECT_TYPE::TWO_HAND:        // ��� �ָ�
                {
                    Two_Hand();
                    break;
                }
                case CPlayer::OBJECT_TYPE::TWO_OBJECT:      // ��� ������Ʈ (�μ� ����)
                {
                    Two_Object();
                    break;
                }
                case CPlayer::OBJECT_TYPE::RIGHT_OBJECT:    // �Ѽ� ������Ʈ (�Ѽ� ����)
                {
                    Right_Object();
                    break;
                }
            }
        }
        else if (m_tRightHand_State.Get_State() == STATE_RIGHTHAND::KICK) // �÷��̾ �����⸦ �� ���
        {
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::NONE); // �޼�Off
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::KICK); // ������ ������
        }
#pragma endregion
    }
#pragma region �÷��̾ �ٰ� �ִ� ���
    else if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::RUN)
    {
        m_eRIGHTState = STATE_RIGHTHAND::RUN_HAND;
        // �����͸� �� ä�� �ٴ� ���
        if (bRighter)
        {
            // �޼� ������ �޸���
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_RIHGTER);
            // ������ �Ǽ� �޸���
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
        // �����͸� Ű�� ���� ���
        else
        {
            // ��� �� �Ǽ� �޸���
            m_tLeftHand_State.Set_State(STATE_LEFTHAND::RUN_HAND);
            m_tRightHand_State.Set_State(STATE_RIGHTHAND::RUN_HAND);
        }
    }
#pragma endregion

}

void CPlayer::State_Update(float fTimeDelta)
{
    // �÷��̾ ������� ��
    if (m_tPlayer_State.Get_State() != STATE_PLAYER::DIE)
    {
        // �÷��̾��� ����
        m_tPlayer_State.Get_StateFunc()(this, fTimeDelta);

        // �÷��̾� �� ���� üũ
        Hand_Check();

        // �÷��̾��� �޼� ����
        m_tLeftHand_State.Get_StateFunc()(this, fTimeDelta);

        // �÷��̾��� ������ ����
        m_tRightHand_State.Get_StateFunc()(this, fTimeDelta);
    }
    else
    {
        // �÷��̾� ����
    }
}

#pragma region �÷��̾��� ����
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
        float fPHeight = 4.0f; // ���� ����

        _vec3 vSpeed = { 0.f, -fPHeight, 0.f };

        float fTime = 1.f * fTimeDelta;
        // ����
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

#pragma region �÷��̾��� �޼� ����

void CPlayer::Left_None(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �޼� ��� Off
        m_tLeftHand.bLeftHandOn = false;
        // �޼� ���� ������ �ʱ�ȭ
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
        // ������ �ָ� �̹���
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_Hand");
        //m_fLeftMaxFrame = 2.f;  // �ִ� ������ ����
        bRighter = false;       // ������ Off

        // ó�� ������ �ִϸ��̼�
        if (m_tLeftHand.bPickUpState)
        {
            // �ִϸ��̼� �ҷ�����
            if (bLeftGetAnimation)
            {
                LeftLoadAnimationFromFile("LeftHandPickUp");
            }
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // �÷��̾ ������ �� �ϰ��
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
        {
            m_tLeftHand.bLeftHandOn = false; // �޼�Off
        }

        // ������ �ִϸ��̼��� �� ������ ���
        if (!m_tLeftHand.bPickUpState)
        {
            if (bLeftGetAnimation)
            {
                m_tLeftHand.bLeftFrameOn = false;      // �޼� ������ �Ŵ��� Off
                m_tLeftHand.bPickUpState = false;         // ���� ������ �ʱ�ȭ
                m_tLeftHand.fLeftFrame = 0.f;
                m_tTime.fRightChangeTime = 1.3f;
                m_tTime.fLeftCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ

                // �޼� �ָ� �ҷ�����
                m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"LeftFist");
                LeftLoadAnimationFromFile("LeftFist");

                bLeftGetAnimation = false; // Off
            }
        }

        if (m_bLAttackMove)
        {
            m_bLRotStart = true;
        }

        // ���ݽ� ī�޶� ����
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

        // ���� ����On
        if (m_tLeftHand.bLeftAttacColOn)
        {
            m_tLeftHand.bLeftAttacColOn = false;    // ���� ���� Off

            // ���ݸ��� ���� ���
            if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;

            // �ָ԰��� ����
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // ���̾�, ����̽�
                vPos, m_pTransformComp->Get_Look(),    // ������ġ, ����
                                   this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // ��������, ��
                                   0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // �ӵ�, �����ð�, ������, ũ��
            
            m_bAttack = false;  // ���� Off
            bLeftPunch = false;
            bRightPunch = true;
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// �޼� �޸��� ������.ver
void CPlayer::Left_RunRighter(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // ������On (�ִ�������) �״��, ���� �� �Ʒ� �����̰� ������ֱ�
    }

    if (m_tLeftHand_State.Can_Update())
    {

    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

// �Ѽ� ������Ʈ�϶� �޼� ���� �ڵ�
void CPlayer::Left_OpenHand(float fTimeDelta)
{
    if (m_tLeftHand_State.IsState_Entered())
    {
        // �޼� ���� �ڵ�� ����
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"OpenHand");

        //bGetAnimation = false;
        //m_tLeftHand.fLeftFrame = 0.f;
        //m_tLeftHand.bLeftFrameOn = false;
        //m_tTime.fLeftMaxTime = 0.f;

        // ó�� ������ �ִϸ��̼�
        if (m_tLeftHand.bPickUpState)
        {
            // �ִϸ��̼� �ҷ�����
            if (bLeftGetAnimation)
            {
                LeftLoadAnimationFromFile("LeftHandPickUp");
            }
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // ������ �ִϸ��̼��� �� ������ ���
        if (!m_tLeftHand.bPickUpState)
        {
            if (bLeftGetAnimation)
            {
                m_tLeftHand.bLeftFrameOn = false;      // �޼� ������ �Ŵ��� Off
                m_tLeftHand.bPickUpState = false;      // ���� ������ �ʱ�ȭ
                m_tLeftHand.fLeftFrame = 0.f;
                m_tTime.fLeftCurrentTime = timeline[KEYTYPE_LEFTHAND].back().time;        // ���� �ð� �ʱ�ȭ

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
        // �޼� �ٴ� ������ ����
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Left_RunHand");

        // �ٴ� �ִϸ��̼�
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
        // �޼��� �����ͷ� ����
        m_pLeftHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Righter");
        
        // �ִϸ��̼� �ҷ�����
        if (bLeftGetAnimation)
        {
            LeftLoadAnimationFromFile("Righter");
            bLeftGetAnimation = false; // Off
        }
    }

    if (m_tLeftHand_State.Can_Update())
    {
        // ���� �������� �ִ� �����Ӻ��� ũ�ų� ���� ���(�ִϸ��̼��� �� ������ ��)
        if (m_tTime.fLeftCurrentTime >= m_tTime.fLeftMaxTime)
        {
            if (bRighter)
            {
                // ������ ��� Off
                //m_tTime.fLeftCurrentTime = m_tTime.fLeftMaxTime;
            }
            else // �����Ͱ� ������ ���
            {
                // �޼� �ָ�
                m_tLeftHand_State.Set_State(STATE_LEFTHAND::HAND);
            }
        }
    }

    if (m_tLeftHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

#pragma region �÷��̾��� ������ ����

void CPlayer::Right_None(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ó�� ������ �ִϸ��̼�
        if (m_eRightState_Old != m_tRightHand_State.Get_State())
        {

        }
        // ������ �ָ����� ����
        m_tRightHand_State.Set_State(STATE_RIGHTHAND::HAND);

        // ������ ��� Off
        //bRightHandOn = false;

        // ������ ���� ������ �ʱ�ȭ
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
        // ó�� ������ �ִϸ��̼�
        if (m_tRightHand.bPickUpState)
        {
            // ������ �ָ� �̹���
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

            // �ִϸ��̼� �ҷ�����
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("RightHandPickUp");

            }
        }
        //// �÷��̾��� ���°� �����ϰ�� �ִϸ��̼� ����
        //if (m_tPlayer_State.Get_State() == STATE_PLAYER::ATTACK)
        //{
        //    // �ִϸ��̼� �ҷ�����
        //    if (bGetAnimation)
        //    {
        //        RightLoadAnimationFromFile("RightFist");
        //        bGetAnimation = false; // Off
        //    }
        //}
    }

    if (m_tRightHand_State.Can_Update())
    {
        // ������ �ִϸ��̼��� �� ������ ���
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // ������ ������ �Ŵ��� Off
                m_tRightHand.fRightFrame = 0.f;         // ���� ������ �ʱ�ȭ
                m_tTime.fRightCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ
                m_tTime.fRightChangeTime = 1.3f;
                // ������ �ָ� �ҷ�����
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");
                RightLoadAnimationFromFile("RightFist");

                bRightGetAnimation = false; // Off
            }
        }

        if (m_bRAttackMove)
        {
            m_bRotStart = true;
        }

        // ���ݽ� ī�޶� ����
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

        // ���� ����On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // ���� ���� Off

            // ���ݸ��� ���� ���
            if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"Punch (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;
            // �ָ԰��� ����
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // ���̾�, ����̽�
                vPos, m_pTransformComp->Get_Look(),    // ������ġ, ����
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // ��������, ��
                0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // �ӵ�, �����ð�, ������, ũ��

            m_bAttack = false;  // ���� Off
            bLeftPunch = true;
            bRightPunch = false;
        }

        // �÷��̾ ��¡�� �ϰ����� ���
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::CHARGING)
        {
            bChargeAttack = true;

            // ������ ������ On
            m_tRightHand.bRightFrameOn = true;

            // ��¡ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"RightHand_Charging");

            m_tTime.fRightChangeTime = 1.2f;         // ������ �ӵ� ����

            // �ҷ����� On
            bRightGetAnimation = true;

            // �ִϸ��̼� �ҷ�����
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

        // �⺻ ���� �ٽ� �ҷ�����
        if (m_tRightHand.bBasicAttackOn)
        {
            // ������ �ָ� �ٽ� �ҷ�����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_Hand");

            m_tTime.fRightChangeTime = 1.f;         // ������ �ӵ� ����
            RightLoadAnimationFromFile("RightFist");
            m_tRightHand.bBasicAttackOn = false;
        }

        // �÷��̾ ������ �� �ϰ��
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::THROW_AWAY)
        {
            // ������ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"UnderThrow_RightHand");
            //m_fRightMaxFrame = 0.f;  // �ִ� ������ ����
            //m_tRightHand.fRightFrameSpeed = 4.f;  // ������ �ӵ� ���� (���� �ӵ�)
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
        // ������ �ٴ� ������ ����
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Right_RunHand");

        // �ٴ� �ִϸ��̼�
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
        // ó�� ������ �ִϸ��̼�
        if (m_tRightHand.bPickUpState)
        {
            // ������ �� ȸ�� �̹���
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun_Spin");

            // �ִϸ��̼� �ҷ�����
            if (bRightGetAnimation)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                //Engine::Play_Sound(L"FallenAces", L"GunSpin (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

                RightLoadAnimationFromFile("GunPickUp");
                m_tTime.fRightChangeTime = 1.5f; // ������ �ӵ� ����
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // �� ȸ���� �� ������ ���
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // ������ ������ �Ŵ��� Off
                m_tRightHand.fRightFrame = 0.f;         // ���� ������ �ʱ�ȭ
                m_tTime.fRightCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ
                m_tTime.fRightChangeTime = 2.f;         // ������ �ӵ� ����

                // ������ �� �ҷ�����
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Gun");
                RightLoadAnimationFromFile("Gun");

                bRightGetAnimation = false; // Off
            }
        }

        if (m_bRAttackMove)
        {
            m_bRotStart = true;
        }

        // ���ݽ� ī�޶� ����
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

        // ���� ����On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // ���� ���� Off

            m_bGunLight = TRUE; // �� ����On

            //CGameObject* pDst = RayCast();

            // ���� ����  (������, �� ������ ���� Ÿ��, �� ������ ����)
            //RayAttack(pDst , -10.f, m_eAttackState);

            // ���ݸ��� ���� ���
            if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"GunFire (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"GunFire (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"GunFire (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"GunFire (3).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"GunFire (4).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"GunFire (5).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();

            // �Ѿ� ����
            Engine::Add_GameObject(L"GameLogic", CPlayerBullet::Create(m_pGraphicDev,                // ���̾�, ����̽�
                vPos, m_pTransformComp->Get_Look(),    // ������ġ, ����
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // ��������, ��
                100.f, 1.f, 10.f, 1.f, m_eRIGHTState));                                           // �ӵ�, �����ð�, ������, ũ��

            Engine::Add_GameObject(L"GameLogic", CEffect_CartridgeCase::Create(m_pGraphicDev,                // ���̾�, ����̽�
                m_pTransformComp->Get_Pos().x + 1.f, m_pTransformComp->Get_Pos().y + 0.5f, m_pTransformComp->Get_Pos().z, 
                m_pTransformComp->Get_Right(), m_pOwner));

            m_bAttack = false;  // ���� Off
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {
        // ���°� �ٲ��� �ٸ� ������ �ǳʰ� �� �������� �ִϸ��̼�
    }
}

void CPlayer::Right_Thompson(float fTimeDelta)
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ó�� ������ �ִϸ��̼�
        if (m_tRightHand.bPickUpState)
        {
            // ������ �轼 �̹���
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");

            // �ִϸ��̼� �ҷ�����
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("ThompsonPickUp");
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // �轼 �����°� �� ������ ���
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation) // �ִϸ��̼� ���� On
            {
                m_tRightHand.bRightFrameOn = false;     // ������ ������ �Ŵ��� Off
                m_tRightHand.fRightFrame = 0.f;         // ���� ������ �ʱ�ȭ
                m_tTime.fRightCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ
                m_tTime.fRightChangeTime = 3.f;         // ������ �ӵ� ����

                // ������ ���ο� �̹����� ����
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Thompson");
                RightLoadAnimationFromFile("Thompson"); // ���ο� �ִϸ��̼� �ε�

                bRightGetAnimation = false; // �ִϸ��̼� ���� Off
            }
        }

        // Ÿ�ݻ��� �ӵ� ����
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

        // ���ݽ� ī�޶� ����
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

        // ���� ����On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // ���� ���� Off

            m_bGunLight = TRUE; // �� ����On

            //CGameObject* pDst = RayCast();

            //// ���� ����  (������, �� ������ ���� Ÿ��, �� ������ ����)
            //RayAttack(pDst, -10.f, m_eAttackState);

            _vec3 vPos = m_pTransformComp->Get_Pos();

            // ���� ex)
            // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
            Engine::Play_Sound(L"FallenAces", L"ThompsonFire (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);


            // �Ѿ� ����
            Engine::Add_GameObject(L"GameLogic", CPlayerBullet::Create(m_pGraphicDev, // ���̾�, ����̽�
                vPos, m_pTransformComp->Get_Look(),             // ������ġ, ����
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),   // ��������, ��
                100.f, 1.f, 40.f, 1.f, m_eRIGHTState));                        // �ӵ�, �����ð�, ������, ũ��f

            m_bAttack = false;  // ���� Off
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
        // ó�� ������ �ִϸ��̼�
        if (m_tRightHand.bPickUpState)
        {
            // ������ ��������
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");

            // �ִϸ��̼� �ҷ�����
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("Steel_PipePickUp");
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // ������ ����� ������ ���
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // ������ ������ �Ŵ��� Off
                m_tRightHand.fRightFrame = 0.f;         // ���� ������ �ʱ�ȭ
                m_tTime.fRightCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ
                m_tTime.fRightChangeTime = 1.3f;

                // ������ �������� �ҷ�����
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe");
                RightLoadAnimationFromFile("Steel_Pipe");

                bRightGetAnimation = false; // Off
            }
        }

        // Ÿ�ݻ��� �ӵ� ����
        if (m_bAttack)
        {
            fAttackTime += 1.f * fTimeDelta;

            if (fAttackTime > 0.8f)
            {
                m_tRightHand.bRightAttacColOn = true;
                fAttackTime = 0.f;
            }
        }

        // ���� ����On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // ���� ���� Off

            // ���ݸ��� ���� ���
            if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"SteelPipe (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"SteelPipe (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"SteelPipe (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;
            // ������ ���� ����
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // ���̾�, ����̽�
                vPos, m_pTransformComp->Get_Look(),    // ������ġ, ����
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // ��������, ��
                0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // �ӵ�, �����ð�, ������, ũ��

            m_bAttack = false;  // ���� Off
        }

        // �÷��̾ ��¡�� �ϰ����� ���
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::CHARGING)
        {
            bChargeAttack = true;

            // ������ ������ On
            m_tRightHand.bRightFrameOn = true;

            // ��¡ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Steel_Pipe_Charging");

            // �ҷ����� On
            bRightGetAnimation = true;

            //m_tTime.fRightChangeTime = 1.2f;         // ������ �ӵ� ����

            // �ִϸ��̼� �ҷ�����
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

        // �⺻ ���� �ٽ� �ҷ�����
        if (m_tRightHand.bBasicAttackOn)
        {
            // ������ �ָ� �ٽ� �ҷ�����
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
        // ó�� ������ �ִϸ��̼�
        if (m_tRightHand.bPickUpState)
        {
            // ������ ���ֺ�
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");

            // �ִϸ��̼� �ҷ�����
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("Steel_PipePickUp");
            }
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // ������ ����� �� ������ ���
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // ������ ������ �Ŵ��� Off
                m_tRightHand.fRightFrame = 0.f;         // ���� ������ �ʱ�ȭ
                m_tTime.fRightCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ
                m_tTime.fRightChangeTime = 1.5f;
                
                // ������ ���ֺ� �ҷ�����
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"BeerBottle");
                RightLoadAnimationFromFile("Beer_Botle");

                bRightGetAnimation = false; // Off
            }
        }

        // ������ �������� �� ������ ���
        if (m_tTime.fRightCurrentTime > m_tTime.fRightMaxTime)
        {
                                        // �ӵ�, �����ð�, ������, ũ��
        }

        // ���� ����On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // ���� ���� Off

            // ���ֺ�(���� ��ų) ����� PlayerLightning���� ���

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;

            CGameObject* pGmaeObj = nullptr;
            CAceMonster* pAceMonster = nullptr;

            pGmaeObj = RayCast();

            if (pGmaeObj != nullptr)
                pAceMonster = dynamic_cast<CAceMonster*>(pGmaeObj);
            //
           // // ���� or ���� ��ų ���� (����Ʈ���ε� ���߰����� ���� �۵�, ������� �����ϸ� �ȳ���, ���������� ����)
            if (pAceMonster != nullptr)
            {
                if(m_bMagicBottle)
                    Engine::Add_GameObject(L"GameLogic", CPlayerLightning::Create(m_pGraphicDev, pAceMonster)); // ����̽�, ���� 
                else
                {                // ���ֺ� ���� ����
                    Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // ���̾�, ����̽�
                        vPos, m_pTransformComp->Get_Look(),    // ������ġ, ����
                        this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                       // ��������, ��
                        100.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                            // �ӵ�, �����ð�, ������, ũ��

                    m_bAttack = false;  // ���� Off
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
        // ó�� ������ �ִϸ��̼�
        if (m_tRightHand.bPickUpState)
        {
            // ������ ��������
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");

            // �ִϸ��̼� �ҷ�����
            if (bRightGetAnimation)
            {
                RightLoadAnimationFromFile("FryingPanPickUp");
            }
            
        }
    }

    if (m_tRightHand_State.Can_Update())
    {
        // �� ȸ���� �� ������ ���
        if (!m_tRightHand.bPickUpState)
        {
            if (bRightGetAnimation)
            {
                m_tRightHand.bRightFrameOn = false;     // ������ ������ �Ŵ��� Off
                m_tRightHand.fRightFrame = 0.f;         // ���� ������ �ʱ�ȭ
                m_tTime.fRightCurrentTime = 0.f;        // ���� �ð� �ʱ�ȭ
                m_tTime.fRightChangeTime = 1.3f;

                // ������ �� �ҷ�����
                m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
                RightLoadAnimationFromFile("FryingPan");

                bRightGetAnimation = false; // Off
            }
        }

        // ���� ����On
        if (m_tRightHand.bRightAttacColOn)
        {
            m_tRightHand.bRightAttacColOn = false;    // ���� ���� Off

            // ���ݸ��� ���� ���
            if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"FryingPan (0).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);
            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"FryingPan (1).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }
            else if (true)
            {
                // ���� ex)
                // �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
                Engine::Play_Sound(L"FallenAces", L"FryingPan (2).wav", SOUND_PLAYER_EFFECT, m_tPlayerSound.m_fSoundVolume);

            }

            _vec3 vPos = m_pTransformComp->Get_Pos();
            vPos.y += 0.7f;

            // �Ѿ� ����
            Engine::Add_GameObject(L"GameLogic", CCloseAttack::Create(m_pGraphicDev,                // ���̾�, ����̽�
                m_pTransformComp->Get_Pos(), m_pTransformComp->Get_Look(),    // ������ġ, ����
                this, m_eAttackState, (ETEAM_ID)Get_TeamID(),                    // ��������, ��
                0.f, 1.f, 10.f, 3.f, m_eRIGHTState));                                           // �ӵ�, �����ð�, ������, ũ��

            m_bAttack = false;  // ���� Off
        }

        // �÷��̾ ��¡�� �ϰ����� ���
        if (m_tPlayer_Action.Get_State() == STATE_PLAYER_ACTION::CHARGING)
        {
            bChargeAttack = true;

            // ������ ������ On
            m_tRightHand.bRightFrameOn = true;

            // ��¡ �ؽ�ó�� ����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan_Charging");

            // �ҷ����� On
            bRightGetAnimation = true;

            // �ִϸ��̼� �ҷ�����
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

        // �⺻ ���� �ٽ� �ҷ�����
        if (m_tRightHand.bBasicAttackOn)
        {
            // ������ �ָ� �ٽ� �ҷ�����
            m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"FryingPan");
            RightLoadAnimationFromFile("FryingPan");
            m_tRightHand.bBasicAttackOn = false;
        }
    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

void CPlayer::Right_Kick(float fTimeDelta) // @@@�Ⱦ�@@@
{
    if (m_tRightHand_State.IsState_Entered())
    {
        // ������ ������
        m_eRIGHTState = STATE_RIGHTHAND::KICK;
        m_pRightHandComp->Receive_Texture(TEX_NORMAL, L"Player", L"Kick");
        //m_fRightMaxFrame = 4.f; // �ִ� ������ ����
        //m_tRightHand.fRightFrameSpeed = 7.f; // ������ �ӵ� ���� (���� �ӵ�)
    }

    if (m_tRightHand_State.Can_Update())
    {

    }

    if (m_tRightHand_State.IsState_Exit())
    {

    }
}

#pragma endregion

// �޼� �ִϸ��̼� �ҷ�����
void CPlayer::LeftLoadAnimationFromFile(const char* fileName)
{
    // .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // ������ UTF-8�� ����
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // ������ �� �� ���� ���� ���� ó��
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

#pragma region ������ Ű���������� ������ ����
    // �ִ� ������ ����
    m_tTime.fLeftMaxTime = timeline[KEYTYPE_LEFTHAND].back().time;
#pragma endregion  
}

// ������ �ִϸ��̼� �ҷ�����
void CPlayer::RightLoadAnimationFromFile(const char* fileName)
{
    // .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // ������ UTF-8�� ����
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) 
    {
        // ������ �� �� ���� ���� ���� ó��
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

#pragma region ������ Ű���������� ������ ����
    // �ִ� ������ ����
    m_tTime.fRightMaxTime = timeline[KEYTYPE_RIGHTHAND].back().time;
#pragma endregion  
}

void CPlayer::LeftInterpolation() // �޼�, ������ ���� ���� �Լ� ������ �����
{
    if (!timeline[KEYTYPE_LEFTHAND].empty()) // �ش� ŰŸ���� ������� üũ
    {
        if (m_tTime.fLeftCurrentTime >= 0.f &&
            m_tTime.fLeftCurrentTime <= timeline[KEYTYPE_LEFTHAND].back().time)
        {
            // ������ 0���� �ʱ�ȭ
            _uint iFrameIndex = 0U;

            // �������� ���������� �����ؼ� ã��
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
                // Ű �����Ӱ� �ð� ��ȭ��
                fFrameTimeDelta = (timeline)[KEYTYPE_LEFTHAND][iFrameIndex + 1U].time - (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].time;

                // ���� Ű �����ӽð����� ���� �ð� ��ȭ��
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
                0.f });	// �̹��� ��ġ

                m_pLeftHandComp->Set_Scale({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.x + fSizeX_Delta, 	// �̹��� ũ��
                                                (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.y + fSizeY_Delta,
                                                1.f });

                m_pLeftHandComp->Set_Rotation({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.x + fRotX_Delta, 	// �̹��� ȸ��
                                                    (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.y + fRotY_Delta,
                                                    (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.z + fRotZ_Delta });

                // �ؽ�ó ��ȣ
                m_tLeftHand.fLeftFrame = _float((timeline)[KEYTYPE_LEFTHAND][iFrameIndex].texureframe);
            }
            else
            {
                m_pLeftHandComp->Set_Scale({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.x, 	// �̹��� ũ��
                    (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vScale.y,
                    1.f });

                m_pLeftHandComp->Set_Rotation({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.x, 	// �̹��� ȸ��
                        (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.y,
                        (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vRot.z });


                m_pLeftHandComp->Set_Pos({ (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.x,
                                            (timeline)[KEYTYPE_LEFTHAND][iFrameIndex].vPos.y + m_vIdlePos.y,
                                            0.f });	// �̹��� ��ġ

                // �ؽ�ó ��ȣ
                m_tLeftHand.fLeftFrame = _float((timeline)[KEYTYPE_LEFTHAND][iFrameIndex].texureframe);
            }
        }
    }
}


void CPlayer::RightInterpolation() // �޼�, ������ ���� ���� �Լ� ������ �����
{
    if (!timeline[KEYTYPE_RIGHTHAND].empty()) // �ش� ŰŸ���� ������� üũ
    {
            if (m_tTime.fRightCurrentTime >= 0.f &&
                m_tTime.fRightCurrentTime <= timeline[KEYTYPE_RIGHTHAND].back().time)
            {
                // ������ 0���� �ʱ�ȭ
                _uint iFrameIndex = 0U;

                // �������� ���������� �����ؼ� ã��
                for (_uint j = _uint(timeline[KEYTYPE_RIGHTHAND].size()) - 1; j >= 0; j--)
                {
                    // Ǯ������ Ű������
                    if (timeline[KEYTYPE_RIGHTHAND][j].bFullChargeKeyframe)
                    {
                        m_tRightHand.iFullChargingIndex = j; // Ǯ���� �ε���
                    }

                    // ����� Ű������
                    if (timeline[KEYTYPE_RIGHTHAND][j].bFullChargeKeyframe)
                    {
                        m_tRightHand.iShieldIndex = j; // ����� �ε���
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
                    // Ű �����Ӱ� �ð� ��ȭ��
                    fFrameTimeDelta = (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex + 1U].time - (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].time;

                    // ���� Ű �����ӽð����� ���� �ð� ��ȭ��
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
                    0.f });	// �̹��� ��ġ

                    m_pRightHandComp->Set_Scale({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.x + fSizeX_Delta, 	// �̹��� ũ��
                                                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.y + fSizeY_Delta,
                                                    1.f });

                    m_pRightHandComp->Set_Rotation({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.x + fRotX_Delta, 	// �̹��� ȸ��
                                                        (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.y + fRotY_Delta,
                                                        (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.z + fRotZ_Delta });

                    // �ؽ�ó ��ȣ
                    m_tRightHand.fRightFrame = _float((timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].texureframe);
                        
                }
                else
                {
                    m_pRightHandComp->Set_Scale({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.x, 	// �̹��� ũ��
                                                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vScale.y,
                                                    1.f });

                    m_pRightHandComp->Set_Rotation({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.x, 	// �̹��� ȸ��
                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.y,
                    (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vRot.z });

                    m_pRightHandComp->Set_Pos({ (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.x,
                                                (timeline)[KEYTYPE_RIGHTHAND][iFrameIndex].vPos.y + m_vIdlePos.y,
                                                0.f });	// �̹��� ��ġ

                    // �ؽ�ó ��ȣ
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
                cout << " �Ÿ� : " << fDist << endl;
                CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDstObj);
                // Ÿ�� ����
                //CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);

                //cout << " ü�� : " << pMonster->Get_MonsterHP().Cur << endl;

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
    // Weapon ��ŷ 
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
    // Food ��ŷ 
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

    // Pow ����
    Engine::Add_GameObject(L"GameLogic", CEffect_HitPow::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 0.2f, m_pTransformComp->Get_Pos().z, this));


    if (pAceObj == nullptr)
        return;

    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
    {
        // Ÿ�� ����
        CAceMonster* pMonster = dynamic_cast<CAceMonster*>(_pDst);

        // ���ݹ��� ����(Ÿ��)�� ���� ���
        if (nullptr != pMonster)
        {
            // �̺�Ʈ �߻�
            pMonster->OnCollisionEntered(this, nullptr);

            // ���� ü�� �޾ƿ���
            m_fMonsterHp = pMonster->Get_MonsterHP();

            // ���� ü�� ���
            m_fMonsterHp.Cur += _fAttack;

            if (m_fMonsterHp.Cur <= 0)
                m_fMonsterHp.Cur = 0.f;
            if (m_fMonsterHp.IsMax())
                m_fMonsterHp.Cur = m_fMonsterHp.Max;

            pMonster->Set_MonsterHP(m_fMonsterHp); // ����� ���� ������ ü������ �־���
            pMonster->Set_Player_AttackState(_eAttackState); // ���ݹ��� ������ �־���
        }
    }
}

//void Ray CPlayer::Event()
//{
//    return void Ray();
//}
