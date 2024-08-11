#include "stdafx.h"
#include "Terrain.h"

#include "Export_System.h"
#include "Export_Utility.h"

using namespace Engine;

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : Base(rhs)
{
}

CTerrain::~CTerrain()
{
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, const char* pTerrainFileName)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(pTerrainFileName)))
    {
        Safe_Release(pInstance);

        MSG_BOX("Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CTerrain::Free()
{
    SUPER::Free();
}


HRESULT CTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_ScaleX(1.f);

    return S_OK;
}

HRESULT CTerrain::Ready_GameObject(const char* pTerrainFileName)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    string strTerrainName = pTerrainFileName;
    wstring strConvert(strTerrainName.begin(), strTerrainName.end());

    Engine::Ready_Proto((L"Proto_" + strConvert + L"TerrainBufferComp").c_str(),
        CTerrainBufferComp::Create(m_pGraphicDev, ("./Resource/Data/Terrain/" + strTerrainName + ".aterrain").c_str(),
            L"./Resource/Texture/Terrain/FlatLand.bmp"));

    Delete_Component(m_pBufferComp);
    m_pBufferComp = Set_DefaultComponent_FromProto<CTerrainBufferComp>(
            ID_STATIC, L"Com_Buffer", (L"Proto_" + strConvert + L"TerrainBufferComp").c_str());

    return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Engine::Add_RenderGroup(RENDER_NONALPHA, this);

    return 0;
}

void CTerrain::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTerrain::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    SetUp_Material();
    m_pTextureComp->Render_Texture(0);
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CTerrain::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CTerrainBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_TerrainBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_TerrainTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

    return S_OK;
}

HRESULT CTerrain::SetUp_Material()
{
    D3DLIGHT9		tLightInfo;
    ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

    tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

    tLightInfo.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tLightInfo.Specular = { 1.f, 1.f, 1.f, 1.f };
    tLightInfo.Ambient = { 1.f, 1.f, 1.f, 1.f };
    tLightInfo.Direction = { 1.f, -1.f, 1.f };

    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

    D3DMATERIAL9			tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    //ambient + diffuse*intensity +specular

    tMtrl.Diffuse = D3DXCOLOR(DXCOLOR_WHITE); //난반사 - 물체의 색이 가장 밝은색임
    tMtrl.Specular = D3DXCOLOR(DXCOLOR_WHITE); //정반사 - 하이라이트 표현
    tMtrl.Ambient = D3DXCOLOR(DXCOLOR_BLACK); //간접조명 - 빛이없더라도 이건 존재한다는 가정하에 들어가는 조명 
    //tMtrl.Ambient = { 0.1f, 0.1f, 0.1f, 1.f };
    tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f }; //자체 발광 
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}
