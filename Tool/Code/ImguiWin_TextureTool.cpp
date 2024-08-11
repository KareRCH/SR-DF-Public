#include "ImguiWin_TextureTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>
#include <imgui_internal.h>

CImguiWin_TextureTool::CImguiWin_TextureTool()
{
}

CImguiWin_TextureTool::~CImguiWin_TextureTool()
{
}

CImguiWin_TextureTool* CImguiWin_TextureTool::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("CImguiWin_TextureTool Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiWin_TextureTool::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_TextureTool::Ready_ImguiWin()
{


    return S_OK;
}

_int CImguiWin_TextureTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_TextureTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            ImVec2 vDockOriginSize = ImGui::GetWindowSize();


            
            ImGuiID dock_texture_key = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 250.f / vDockOriginSize.y, NULL, &dockspace_id);
            ImGuiID dock_texture_addkey = ImGui::DockBuilderSplitNode(dock_texture_key, ImGuiDir_Left, 250.f / vDockOriginSize.x, NULL, &dock_texture_key);
            ImGuiID dock_texture_file = ImGui::DockBuilderSplitNode(dock_texture_key, ImGuiDir_Right, 0.4f, NULL, &dock_texture_key);
            ImGui::DockBuilderDockWindow(u8"텍스처 키 추가", dock_texture_addkey);
            ImGui::DockBuilderDockWindow(u8"텍스처 키", dock_texture_key);
            ImGui::DockBuilderDockWindow(u8"텍스처 정보", dock_texture_file);

            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 250.f / vDockOriginSize.x, NULL, &dockspace_id);
            ImGui::DockBuilderDockWindow(u8"뷰어", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"그룹", dock_right_id);


            

            ImGui::DockBuilderFinish(dockspace_id);

            m_bFirstLoop = false;
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }


    // 리소스
    if (ImGui::Begin(u8"텍스처 키 추가", NULL, iMain_Flags))
    {


    }   ImGui::End();

    if (ImGui::Begin(u8"텍스처 키", NULL, iMain_Flags))
    {


    }   ImGui::End();

    if (ImGui::Begin(u8"텍스처 정보", NULL, iMain_Flags))
    {


    }   ImGui::End();

    // 계층
    if (ImGui::Begin(u8"그룹", NULL, iMain_Flags))
    {


    }   ImGui::End();

    // 뷰어
    if (ImGui::Begin(u8"뷰어", NULL, iMain_Flags))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));

    }   ImGui::End();



    ImGui::End();

    return 0;
}
