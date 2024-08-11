#include "ImguiWin_Test.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>

CImguiWin_Test::CImguiWin_Test()
{
}

CImguiWin_Test::~CImguiWin_Test()
{
}

CImguiWin_Test* CImguiWin_Test::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BackGround Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiWin_Test::Free()
{
    SUPER::Free();

    if (m_pTexture != nullptr)
        m_pTexture->Release();
    if (m_pRenderTargetSurface != nullptr)
        m_pRenderTargetSurface->Release();
}

HRESULT CImguiWin_Test::Ready_ImguiWin()
{
    return S_OK;
}

_int CImguiWin_Test::Update_ImguiWin(const _float& fTimeDelta)
{
    /*if (m_pTexture != nullptr)
        m_pTexture->Release();
    if (m_pRenderTargetSurface != nullptr)
        m_pRenderTargetSurface->Release();*/

    /*LPDIRECT3DDEVICE9 pGraphicDev = CImguiMgr::GetInstance()->Get_GraphicDev();
    pGraphicDev->GetRenderTarget(0, &m_pRenderTargetSurface);
    pGraphicDev->CreateTexture(WINCX, WINCY, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL);
    m_pTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);*/
    
    //m_pTexture = *CImguiMgr::GetInstance()->Get_EditorTexture();

    ImGui::Begin(u8"응애 머신");

    /*if (m_pTexture)
    {
        ImGui::Image((void*)m_pTexture, ImVec2(WINCX, WINCY));
        m_pTexture->Release();
        m_pTexture = nullptr;
    }*/

    ImGui::End();
        

    return 0;
}
