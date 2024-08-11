#include "stdafx.h"
#include "AceBuilding.h"



CAceBuilding::CAceBuilding(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceBuilding::CAceBuilding(const CAceBuilding& rhs)
    : Base(rhs)
{
}

CAceBuilding::~CAceBuilding()
{
}

CAceBuilding* CAceBuilding::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
                                    const _tchar* pObjTag, 
                                    const _float _fx, const _float _fy, const _float _fz, 
                                    CAceObjectFactory::OBJECT_DIRECTION pDirection)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos({ _fx, _fy, _fz });
    return pInstance;
}

CAceBuilding* CAceBuilding::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

HRESULT CAceBuilding::Ready_GameObject()
{
    SUPER::Ready_GameObject();
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CAceBuilding::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] =  tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    if (!tObjectSerial.strGroupKey.empty() && !tObjectSerial.strTextureKey.empty())
    m_pTextureComp->Receive_Texture(TEX_CUBE,
        wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str()
        , wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    //ObjectName(wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    return S_OK;
}

_int CAceBuilding::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 물리바디 업데이트
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceBuilding::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceBuilding::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pCubeBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CAceBuilding::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}

void CAceBuilding::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceBuilding::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceBuilding::OnCollisionExited(CGameObject* pDst)
{
}


void CAceBuilding::Free()
{
    SUPER::Free();
}