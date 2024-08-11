#include "CubeObject.h"

#include "Export_System.h"
#include "Export_Utility.h"

CCubeObject::CCubeObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CCubeObject::CCubeObject(const CCubeObject& rhs)
    : Base(rhs)
{
}

CCubeObject::~CCubeObject()
{
}

CCubeObject* CCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(vPos, vRot, vScale, strGroupKey, strTextureKey)))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

CCubeObject* CCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

void CCubeObject::Free()
{
    SUPER::Free();
}

HRESULT CCubeObject::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CCubeObject::Ready_GameObject(const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(vPos);
    m_pTransformComp->Set_Rotation(vRot);
    m_pTransformComp->Set_Scale(vScale);

    if (!strGroupKey.empty() && !strTextureKey.empty())
    m_pTextureComp->Receive_Texture(TEX_CUBE,
        wstring(strGroupKey.begin(), strGroupKey.end()).c_str()
        , wstring(strTextureKey.begin(), strTextureKey.end()).c_str());

    return S_OK;
}

HRESULT CCubeObject::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(tObjectSerial.vRotation);
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] =  tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    m_pTextureComp->Receive_Texture(TEX_CUBE,
        wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str()
        , wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    return S_OK;
}

_int CCubeObject::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // Renderer µî·Ï 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CCubeObject::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CCubeObject::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    DWORD hrColor;
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &hrColor);
    if (m_bIsSelected)
    {
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, DXCOLOR_MAGENTA);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); 

    m_pTextureComp->Render_Texture(0);
    m_pCubeBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, hrColor);

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CCubeObject::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BuildingTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_CubeBufferComp"), E_FAIL);

    return S_OK;
}

