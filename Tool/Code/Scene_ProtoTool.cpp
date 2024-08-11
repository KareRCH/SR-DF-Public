#include "Scene_ProtoTool.h"

CScene_ProtoTool::CScene_ProtoTool(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CScene_ProtoTool::~CScene_ProtoTool()
{
}

CScene_ProtoTool* CScene_ProtoTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene()))
    {
        Safe_Release(pInstance);
        MSG_BOX("MapTool Scene Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CScene_ProtoTool::Free()
{
    SUPER::Free();
}

HRESULT CScene_ProtoTool::Ready_Scene()
{
    return S_OK;
}

_int CScene_ProtoTool::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_ProtoTool::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();
}

void CScene_ProtoTool::Render_Scene()
{
}

HRESULT CScene_ProtoTool::Ready_Layer_Completed()
{
    return S_OK;
}
