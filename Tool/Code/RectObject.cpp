#include "RectObject.h"

#include "Export_System.h"
#include "Export_Utility.h"

CRectObject::CRectObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CRectObject::CRectObject(const CRectObject& rhs)
    : Base(rhs)
{
}

CRectObject::~CRectObject()
{
}

CRectObject* CRectObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(vPos, vRot, vScale, strGroupKey, strTextureKey)))
    {
        Safe_Release(pInstance);

        MSG_BOX("RectObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

CRectObject* CRectObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("RectObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CRectObject::Free()
{
    SUPER::Free();
}

HRESULT CRectObject::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CRectObject::Ready_GameObject(const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(vPos);
    m_pTransformComp->Set_Rotation(vRot);
    m_pTransformComp->Set_Scale(vScale);

    if (!strGroupKey.empty() && !strTextureKey.empty())
        m_pTextureComp->Receive_Texture(TEX_NORMAL,
            wstring(strGroupKey.begin(), strGroupKey.end()).c_str()
            , wstring(strTextureKey.begin(), strTextureKey.end()).c_str());

    return S_OK;
}

HRESULT CRectObject::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
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

    m_pTextureComp->Receive_Texture(TEX_NORMAL,
        wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str()
        , wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    return S_OK;
}

_int CRectObject::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // Renderer µî·Ï 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CRectObject::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CRectObject::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    DWORD hrColor;
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &hrColor);
    if (m_bIsSelected)
    {
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, DXCOLOR_MAGENTA);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    }

    m_pTextureComp->Render_Texture(0);
    m_pRectBufferComp->Render_Buffer();

    if (m_bIsSelected)
    {
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, hrColor);
    }

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CRectObject::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pRectBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_RcTexBufferComp"), E_FAIL);

    return S_OK;
}
