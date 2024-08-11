#include "ImguiMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "ImguiWin.h"
#include "ImguiWin_Test.h"
#include "ImguiAnimationTool.h"
#include "ImguiWin_TextureTool.h"
#include "ImguiWin_DockingSpace.h"
#include "ImguiWin_MapTool.h"
#include "ImguiWin_ProtoTool.h"

IMPLEMENT_SINGLETON(CImguiMgr)

CImguiMgr::CImguiMgr()
{

}

CImguiMgr::~CImguiMgr()
{
	Free();
}

void CImguiMgr::Free()
{
	// ImGui 끝내기
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 장치 제거, RefCount 감소
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	// ImguiWin 컨텐츠들 제거
	for (auto item : m_mapImguiWin)
		Safe_Release(item.second);

	for (auto item : m_vecRenderTargetTex)
		Safe_Release(item);
}

HRESULT CImguiMgr::Ready_Imgui(CGraphicDev** ppGraphicClass, LPDIRECT3DDEVICE9* ppGraphicDev, _uint iRenderTargetCount)
{
	// 이미 설정된 장치값을 받아 설정
	m_pDeviceClass = (*ppGraphicClass)->Get_ByRef<CGraphicDev>();
	m_pGraphicDev = (*ppGraphicDev);
	m_pGraphicDev->AddRef();


	// 렌더 타겟 수량 설정 및 생성
	m_vecRenderTargetTex.reserve(iRenderTargetCount);
	for (_uint i = 0U; i < iRenderTargetCount; i++)
	{
		LPDIRECT3DTEXTURE9 pTexture = nullptr;
		m_pGraphicDev->CreateTexture(m_pDeviceClass->Get_D3DPP()->BackBufferWidth, m_pDeviceClass->Get_D3DPP()->BackBufferHeight,
			1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL);
		m_vecRenderTargetTex.push_back(pTexture);
	}


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_pIO = &ImGui::GetIO(); (void)m_pIO;
	//m_pIO->DisplaySize = {1280, 720};
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_pIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//m_pIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//m_io.ConfigViewportsNoAutoMerge = true;
	//m_io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	//소영 - 한글폰트추가
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("wanju.ttf", 15.f, NULL, io.Fonts->GetGlyphRangesKorean());

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX9_Init(m_pGraphicDev);

	
	// 사용 툴 객체 등록
	//m_mapImguiWin.emplace(L"Test",  CImguiWin_Test::Create());
	m_mapImguiWin.emplace(L"DockingSpace", CImguiWin_DockingSpace::Create());
	m_mapImguiWin.emplace(L"MapTool", CImguiWin_MapTool::Create());
	m_mapImguiWin.emplace(L"TextureTool", CImguiWin_TextureTool::Create());
	m_mapImguiWin.emplace(L"AnimationTool", CImguiAnimationTool::Create());
	m_mapImguiWin.emplace(L"ProtoTool", CImguiWin_ProtoTool::Create());

	Sort_ImguiWin();

	return S_OK;
}

HRESULT CImguiMgr::Update_Imgui(const _float& fTimeDelta)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 컨테이너에 저장된 윈도우 실행
	for (auto item : m_vecSortedImguiWin)
		item.second->Update_ImguiWin(fTimeDelta);

	// 렌더링 옵션
	ImGui::EndFrame();
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	return S_OK;
}

HRESULT CImguiMgr::Render_Imgui()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

HRESULT CImguiMgr::Render_AdditionImgui()
{
	// Update and Render additional Platform Windows
	if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	return S_OK;
}

HRESULT CImguiMgr::Render_LossHandle(HRESULT dwDeviceState)
{
	// 핸들을 잃으면 디바이스를 초기화한다.
	if (dwDeviceState == D3DERR_DEVICELOST && m_pGraphicDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();


	return S_OK;
}

void CImguiMgr::ResetDevice(_uint dwResizeWidth, _uint dwResizeHeight)
{
	if (dwResizeWidth == 0 && dwResizeHeight == 0)
		return;

	m_pDeviceClass->Get_D3DPP()->BackBufferWidth = dwResizeWidth;
	m_pDeviceClass->Get_D3DPP()->BackBufferHeight = dwResizeHeight;

	// 렌더타겟 텍스처 해제
	for (auto itemTexture : m_vecRenderTargetTex)
	{
		Safe_Release(itemTexture);
	}
	m_vecRenderTargetTex.clear();

	// ImGui 리셋하기
	ImGui_ImplDX9_InvalidateDeviceObjects();
	if (m_pDeviceClass->Reset_GraphicDev() == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();

	// 렌더러가 준비되어 있다면, 기본 뷰포트의 사이즈도 변경한다. 
	// 직교도 변경해준다.
	// 뷰포트는 장치 초기화 후에 해줘야함.
	if (Engine::IsReady_Renderer())
	{
		D3DXMatrixOrthoLH(Engine::Get_Renderer()->Get_MatOrthoProject(), (FLOAT)dwResizeWidth, (FLOAT)dwResizeHeight, 0.f, 100.f);
		Engine::Get_Renderer()->Get_Viewport(0).Width = dwResizeWidth;
		Engine::Get_Renderer()->Get_Viewport(0).Height = dwResizeHeight;
	}

	_uint iSize = (dwResizeWidth > dwResizeHeight) ? dwResizeWidth : dwResizeHeight;
	_uint iPower = 1U;
	while (iPower < iSize)
	{
		iPower *= 2U;
	}

	// 렌더타겟 텍스처 재할당
	for (_uint i = 0U; i < m_vecRenderTargetTex.capacity(); i++)
	{
		LPDIRECT3DTEXTURE9 pTexture = nullptr;
		m_pGraphicDev->CreateTexture(
			iPower, iPower,
			1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL);
		m_vecRenderTargetTex.push_back(pTexture);
	}
}

bool CImguiMgr::LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight)
{
	return false;
}

void CImguiMgr::Sort_ImguiWin()
{
	// 우선도 기반으로 루프 돌리기
	m_vecSortedImguiWin.reserve(m_mapImguiWin.size());
	for (auto item : m_mapImguiWin)
		m_vecSortedImguiWin.push_back(item);

	// 정렬
	sort(m_vecSortedImguiWin.begin(), m_vecSortedImguiWin.end(),
		[](pair<const _tchar*, CImguiWin*>& pDst, pair<const _tchar*, CImguiWin*>& pSrc) {
			return (pDst.second->Get_Priority() > pSrc.second->Get_Priority());
		});
}

void CImguiMgr::HelpMarkerEx(const char* marker, const char* desc)
{
	ImGui::TextDisabled(marker);
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void CImguiMgr::Open_Layout(const _tchar* pWin)
{
	auto iter = m_mapImguiWin.find(pWin);
	if (iter == m_mapImguiWin.end())
		return;

	(*iter).second->Set_Open(true);
}

void CImguiMgr::Close_Layout(const _tchar* pWin)
{
	auto iter = m_mapImguiWin.find(pWin);
	if (iter == m_mapImguiWin.end())
		return;

	(*iter).second->Set_Open(false);
	(*iter).second->Set_FirstLoop(true);
}
