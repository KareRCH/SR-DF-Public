#include "ImguiWin_DockingSpace.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>

CImguiWin_DockingSpace::CImguiWin_DockingSpace()
	: m_bOpt_FullScreen(), m_bOpt_Padding(), m_DockSpace_Flags(), m_Window_Flags()
{
}

CImguiWin_DockingSpace::~CImguiWin_DockingSpace()
{
}

CImguiWin_DockingSpace* CImguiWin_DockingSpace::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("ImguiWin_DockingSpace Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiWin_DockingSpace::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_DockingSpace::Ready_ImguiWin()
{
	m_bOpen = true;
	m_bOpt_FullScreen = true;
	m_bOpt_Padding = false;
	m_DockSpace_Flags = ImGuiDockNodeFlags_None;
	m_Window_Flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	m_fPriority = 10.f;

    return S_OK;
}

_int CImguiWin_DockingSpace::Update_ImguiWin(const _float& fTimeDelta)
{
	SUPER::Update_ImguiWin(fTimeDelta);

	m_DockSpace_Flags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;
	m_Window_Flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;

	
	if (m_bOpt_FullScreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		m_Window_Flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		m_Window_Flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.11f, 1.0f));
	}
	else
	{
		m_DockSpace_Flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// �������̽��� ������ �����ǵ��� ����� �÷���, �Ⱦ�����
	if (m_DockSpace_Flags & ImGuiDockNodeFlags_PassthruCentralNode)
		m_Window_Flags |= ImGuiWindowFlags_NoBackground;

	if (!m_bOpt_Padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &m_bOpen, m_Window_Flags);
	if (!m_bOpt_Padding)
		ImGui::PopStyleVar();

	if (m_bOpt_FullScreen)
	{
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(1);
	}

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockSpace_Flags);
	}
	else
	{
		//ShowDockingDisabledMessage();
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open Project");
			ImGui::MenuItem("Save Project");
			ImGui::MenuItem("Exit");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem(u8"����");
			ImGui::EndMenu();
		}


		// �� ���̾ƿ�
		CImguiMgr::GetInstance()->HelpMarkerEx(u8"   ���� ",
			u8"�����ʿ��� ���ϴ� ������ ������ �� �ֽ��ϴ�");

		if (ImGui::BeginTabBar("Layout"))
		{
			if (ImGui::BeginTabItem(u8"�� ��")) 
			{
				CImguiMgr::GetInstance()->Open_Layout(L"MapTool");
				CImguiMgr::GetInstance()->Close_Layout(L"TextureTool");
				CImguiMgr::GetInstance()->Close_Layout(L"AnimationTool");
				CImguiMgr::GetInstance()->Close_Layout(L"ProtoTool");
				// ���� �����Ѵ�.
				FAILED_CHECK_RETURN(Engine::Set_Scene(L"MapTool"), E_FAIL);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(u8"�ؽ�ó ��"))
			{
				CImguiMgr::GetInstance()->Close_Layout(L"MapTool");
				CImguiMgr::GetInstance()->Open_Layout(L"TextureTool");
				CImguiMgr::GetInstance()->Close_Layout(L"AnimationTool");
				CImguiMgr::GetInstance()->Close_Layout(L"ProtoTool");
				// ���� �����Ѵ�.
				FAILED_CHECK_RETURN(Engine::Set_Scene(L"TextureTool"), E_FAIL);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(u8"�ִϸ��̼� ��"))
			{
				CImguiMgr::GetInstance()->Close_Layout(L"MapTool");
				CImguiMgr::GetInstance()->Close_Layout(L"TextureTool");
				CImguiMgr::GetInstance()->Open_Layout(L"AnimationTool");
				CImguiMgr::GetInstance()->Close_Layout(L"ProtoTool");
				// ���� �����Ѵ�.
				FAILED_CHECK_RETURN(Engine::Set_Scene(L"AnimationTool"), E_FAIL);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(u8"���η� ��"))
			{
				CImguiMgr::GetInstance()->Close_Layout(L"MapTool");
				CImguiMgr::GetInstance()->Close_Layout(L"TextureTool");
				CImguiMgr::GetInstance()->Close_Layout(L"AnimationTool");
				CImguiMgr::GetInstance()->Open_Layout(L"ProtoTool");
				// ���� �����Ѵ�.
				FAILED_CHECK_RETURN(Engine::Set_Scene(L"ProtoTool"), E_FAIL);

				ImGui::EndTabItem();
			}

			if (m_bFirstLoop)
			{
				ImGui::SetTabItemClosed(u8"�� ��");
				ImGui::SetTabItemClosed(u8"�ؽ�ó ��");
			}

			ImGui::EndTabBar();
		}

		

		ImGui::EndMenuBar();
	}
	

	// �� ����
	if (m_bFirstLoop)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, m_DockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());


		ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, NULL, &dockspace_id);
		ImGui::DockBuilderDockWindow("Left", dock_right_id);
		ImGui::DockBuilderDockWindow("Center", dockspace_id);
		//ImGui::DockBuilderDockWindow("One", dockspace_id);
		//ImGui::DockBuilderDockWindow("Two", dockspace_id);

		ImGui::DockBuilderFinish(dockspace_id);

		m_bFirstLoop = false;
	}

	/*if (ImGui::Begin("Center"))
	{
		ImGui::Text("Hello");
		ImGui::Separator();
		
	}
	ImGui::End();*/

	ImGui::End();

	m_bFirstLoop = false;

    return 0;
}
