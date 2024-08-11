#include "stdafx.h"
#include "AceInteraction.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include <AceBuilding.h>

CAceInteraction::CAceInteraction(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceInteraction::CAceInteraction(const CAceInteraction& rhs)
	: Base(rhs)
{
}

CAceInteraction::~CAceInteraction()
{
}

CAceInteraction* CAceInteraction::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos({ _fx, _fy, _fz });
    pInstance->ObjectName(pObjTag); // 처음 들어온값을 enum 으로 변경 
    return pInstance;
}

CAceInteraction* CAceInteraction::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CAceInteraction::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_eFactoryClass = OBJECT_CLASS::INTERACTION;

    return S_OK;
}

HRESULT CAceInteraction::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(tObjectSerial.vRotation);
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;


    if (!tObjectSerial.strGroupKey.empty() && !tObjectSerial.strTextureKey.empty())
        m_pTextureComp->Receive_Texture(TEX_NORMAL,
            wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str()
            , wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    return S_OK;
}

HRESULT CAceInteraction::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    
    // 충돌 컴포넌트 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_ITEM); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_WALL); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

    return S_OK;
}

_int CAceInteraction::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);
    
    Gravity(fTimeDelta);

    m_pTransformComp->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

    // 지형타기 
    if (m_pTransformComp->Get_Pos().y < 1.5f && m_vSpeed.y < 0.f)
    {
        Height_On_Terrain();
        m_IsOnGround = true;
    }
    else
        m_IsOnGround = false;
    // 빌보드
    //if((CAceInteraction::INTERACTION_NAME::NEWSPAPER ) <= m_pCurName)
    //BillBoard(fTimeDelta);

    // TEST --------------------------
    if (Engine::IsKey_Pressed(DIK_P))
        m_bOnOff = true;

    //--------------------------------

    // 변수에 저장된 enum으로 texture 결정 - eaten 변경때문에 
    Change_Texture(m_pCurName);

    // 물리바디 업데이트
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceInteraction::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceInteraction::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceInteraction::Free()
{
    SUPER::Free();
}

#pragma region Basic Setting Function 

HRESULT CAceInteraction::BillBoard(const _float& fTimeDelta)
{
    _matrix		matWorld;
    _vec3 vPlayerPos;

    matWorld = *m_pTransformComp->Get_Transform();

    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 Pos = m_pTransformComp->Get_Pos();

    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // 회전행렬 생성
    _matrix rotationMatrix;
    D3DXMatrixRotationY(&rotationMatrix, rad);

    m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

    return S_OK;
}

void CAceInteraction::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
    if (!pTerrainBufferComp)
        return;

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 0.6f + m_fBillboardHeight, vPos.z);
}

void CAceInteraction::ObjectName(const _tchar* pObjTag)
{
    if ((wcscmp(pObjTag, L"LightSwitch") == 0) || (wcscmp(pObjTag, L"LIGHT_SWITCH") == 0))
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"LightSwitch_Off");
        m_pCurName = CAceInteraction::INTERACTION_NAME::LIGHTSWITCH;
        m_pTransformComp->Set_Scale({ 0.25f, 0.5f, 1.f });
        m_fBillboardHeight = 1.5f;
    }
    else if ((wcscmp(pObjTag, L"OneButton") == 0) || (wcscmp(pObjTag, L"ONEBUTTON") == 0))
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"OneButton_Off");
        m_pCurName = CAceInteraction::INTERACTION_NAME::ONE_BUTTON;
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 1.f });
        m_fBillboardHeight = 1.5f;
    }
    else if ((wcscmp(pObjTag, L"TwoButton") == 0) || (wcscmp(pObjTag, L"TWOBUTTON") == 0))
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"TwoButton_Off");
        m_pCurName = CAceInteraction::INTERACTION_NAME::TWO_BUTTON;
        m_pTransformComp->Set_Scale({ 0.5f, 1.f, 1.f });
        m_fBillboardHeight = 1.5f;
    }
    else if ((wcscmp(pObjTag, L"BlockSwitch") == 0) || (wcscmp(pObjTag, L"BLOCK_SWITCH") == 0))
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"BlockSwitch_Off");
        m_pCurName = CAceInteraction::INTERACTION_NAME::BLOCK_SWITCH;
        m_pTransformComp->Set_Scale({ 0.5f, 1.f, 1.f });
        m_fBillboardHeight = 1.5f;
    }
    else if ((wcscmp(pObjTag, L"Padlock") == 0) || (wcscmp(pObjTag, L"PADLOCK") == 0))
    {
        m_pCurName = CAceInteraction::INTERACTION_NAME::PADLOCK;
        m_pTransformComp->Set_Scale({ 3.f, 0.5f, 1.f });
        m_fBillboardHeight = 0.75f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"Padlock_Off");
    }
    else if ((wcscmp(pObjTag, L"Newspaper") == 0) || (wcscmp(pObjTag, L"NEWSPAPER") == 0))
    {
        m_pCurName = CAceInteraction::INTERACTION_NAME::NEWSPAPER;
        m_fBillboardHeight = -0.38f;
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 1.f });
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"Newspaper");
    }
    else if ((wcscmp(pObjTag, L"ComicBook") == 0) || (wcscmp(pObjTag, L"COMICBOOK") == 0))
    {
        m_pCurName = CAceInteraction::INTERACTION_NAME::COMICBOOK;
        m_fBillboardHeight = -0.39f;
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 1.f });
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"ComicBook");
    }
    else if ((wcscmp(pObjTag, L"Building_Key") == 0) || (wcscmp(pObjTag, L"BUILDING_KEY") == 0))
    {
        m_pCurName = CAceInteraction::INTERACTION_NAME::BUILDING_KEY;
        m_fBillboardHeight = -0.5f;
        m_pTransformComp->Set_Scale({ 0.25f, 0.15f, 1.f });
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"Building_Key");
    }
    else if ((wcscmp(pObjTag, L"Bossroom_Key") == 0) || (wcscmp(pObjTag, L"BOSSROOM_KEY") == 0))
    {
        m_pCurName = CAceInteraction::INTERACTION_NAME::BOSSROOM_KEY;
        m_fBillboardHeight = -0.5f;
        m_pTransformComp->Set_Scale({ 0.25f, 0.15f, 1.f });
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"Bossroom_Key");
    }
    else
    {
        m_pCurName = CAceInteraction::INTERACTION_NAME::INTERACTION_END;
    }
}

void CAceInteraction::Change_Texture(INTERACTION_NAME eReceiveName)
{
    if (m_bOnOff)
    {
        switch (eReceiveName)
        {
        case CAceInteraction::INTERACTION_NAME::ONE_BUTTON:
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"OneButton_On");
            break;

        case CAceInteraction::INTERACTION_NAME::TWO_BUTTON:
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"TwoButton_On");
            break;

        case CAceInteraction::INTERACTION_NAME::LIGHTSWITCH:
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"LightSwitch_On");
            break;

        case CAceInteraction::INTERACTION_NAME::BLOCK_SWITCH:
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Interaction", L"BlockSwitch_On");
            break;

        }
    }
}

void CAceInteraction::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
    CAceBuilding* pSolid = dynamic_cast<CAceBuilding*>(pDst);
    if (pSolid)
    {
        _vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
        m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
        if (D3DXVec3Dot(&(-vNormal), &_vec3({ 0.f, -1.f, 0.f })) < 0.f)
            m_IsOnGround = true;
    }
}

void CAceInteraction::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceInteraction::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion 
//--------------------------------------------------------------