#include "Scene_TextureTool.h"

CScene_TextureTool::CScene_TextureTool(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{
}

CScene_TextureTool::~CScene_TextureTool()
{
}

CScene_TextureTool* CScene_TextureTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

void CScene_TextureTool::Free()
{
    SUPER::Free();
}

HRESULT CScene_TextureTool::Ready_Scene()
{


    return Ready_Layer_Completed();
}

_int CScene_TextureTool::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_TextureTool::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();


}

void CScene_TextureTool::Render_Scene()
{
    // µð¹ö±×

}

HRESULT CScene_TextureTool::Ready_Layer_Completed()
{
    return S_OK;
}
