//#ifndef IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_MATH_OPERATORS
//#endif

#include "framework.h"
#include "MainApp.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "ImguiMgr.h"
#include "Logo.h"
#include "Scene_MapTool.h"
#include "Scene_TextureTool.h"
#include "Scene_AnimationTool.h"
#include "Scene_ProtoTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMainApp::CMainApp()
	: m_pDeviceClass(nullptr), m_pGraphicDev(nullptr), m_pManagementClass(nullptr)
{
}

CMainApp::~CMainApp()
{
}

CMainApp* CMainApp::Create()
{
	ThisClass* pInstance = new	 ThisClass;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("MainApp Create Failed");
		Engine::Safe_Release(pInstance);

		return nullptr;
	}
	return pInstance;
}

void CMainApp::Free()
{
	CImguiMgr::GetInstance()->DestroyInstance();

	// 장치 제거
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	// 루트 매니저 클래스 제거
	Safe_Release(m_pManagementClass);

	// dll 싱글톤 제거
	Engine::Release_Utility();
	Engine::Release_System();
}

HRESULT CMainApp::Ready_MainApp()
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN(CImguiMgr::GetInstance()->Ready_Imgui(&m_pDeviceClass, &m_pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	return S_OK;
}

int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	// 다이렉트 인풋 업데이트
	Engine::Update_KeyMgr();
	CImguiMgr::GetInstance()->Update_Imgui(fTimeDelta);
	// 물리 업데이트
	Engine::StartFrame_PhysicsMgr();

	// 씬 업데이트
	m_pManagementClass->Update_Scene(fTimeDelta);

	// 물리 업데이트
	Engine::Update_PhysicsMgr(fTimeDelta);


	
	
	
	return 0;
}

void CMainApp::LateUpdate_MainApp()
{
	Engine::LateUpdate_Scene();
	Engine::LateUpdate_KeyMgr();		// 가상키 시스템 업데이트
}

void CMainApp::Render_MainApp()
{
	HRESULT result = S_OK;
	D3DXCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
	
	// [렌더 시작]
	if ((result = Engine::Render_Begin(clear_col_dx)) >= 0)
	{
		// [오브젝트 렌더] 렌더러에 요청해서 그려야할 오브젝트들을 그린다.
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		Engine::Render_Scene(m_pGraphicDev);
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		LPDIRECT3DSURFACE9 pRenderTarget = nullptr;
		D3DSURFACE_DESC desc;

		pImguiMgr->Get_GraphicDev()->GetBackBuffer(0, 0U, D3DBACKBUFFER_TYPE::D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		pImguiMgr->Get_VecRenderTargetTex(0)->GetSurfaceLevel(0, &pRenderTarget);
		pImguiMgr->Get_GraphicDev()->StretchRect(pBackBuffer, NULL, pRenderTarget, NULL, D3DTEXF_NONE);
		pBackBuffer->GetDesc(&desc);

		Safe_Release(pBackBuffer);
		Safe_Release(pRenderTarget);

		// [IMGUI 렌더]
		if (Engine::IsKey_Pressing(DIK_LCONTROL) && Engine::IsKey_Pressed(DIK_T))
			m_bRenderImgui = !m_bRenderImgui;

		if (m_bRenderImgui)
			CImguiMgr::GetInstance()->Render_Imgui();
	}

	// 추가 뷰포트 렌더
	CImguiMgr::GetInstance()->Render_AdditionImgui();

	// [렌더 종료] 스왑 체인을 하여 백버퍼를 앞으로 불러들이고 종료시킨다.
	result = Engine::Render_End();
	
	// 핸들을 잃었을 때 실행시키는 코드
	CImguiMgr::GetInstance()->Render_LossHandle(result);
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement)
{
	Engine::CScene* pScene = nullptr;

	// 씬 매니지먼트 추가
	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagement, EMANAGE_SCENE::MULTI), E_FAIL);
	(*ppManagement)->AddRef();

	// 기본 씬
	/*NULL_CHECK_RETURN(pScene = CLogo::Create(pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Add_Scene(pScene, L"Logo"), E_FAIL);*/
	NULL_CHECK_RETURN(pScene = CScene_MapTool::Create(pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Add_Scene(pScene, L"MapTool"), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(L"MapTool"), E_FAIL);

	NULL_CHECK_RETURN(pScene = CScene_AnimationTool::Create(pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Add_Scene(pScene, L"AnimationTool"), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(L"AnimationTool"), E_FAIL);

	NULL_CHECK_RETURN(pScene = CScene_TextureTool::Create(pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Add_Scene(pScene, L"TextureTool"), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(L"TextureTool"), E_FAIL);

	NULL_CHECK_RETURN(pScene = CScene_ProtoTool::Create(pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Add_Scene(pScene, L"ProtoTool"), E_FAIL);
	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(L"ProtoTool"), E_FAIL);

	// 씬을 설정한다.
	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(L"AnimationTool"), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(&m_pDeviceClass, g_hWnd, MODE_WIN), E_FAIL);
	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 장치에서 샘플러를 사용할 수 있게 설정하는 옵션 (안티 앨리어싱)
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// InputDev
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// Font
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Default", L"바탕", 15, 20, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Jinji", L"궁서", 30, 30, FW_THIN), E_FAIL);

	// Renderer
	FAILED_CHECK_RETURN(Engine::Ready_Renderer(), E_FAIL);

	// TextureMgr
	FAILED_CHECK_RETURN(Engine::Ready_TextureMgr(*ppGraphicDev), E_FAIL);

	// PhysicsMgr
	FAILED_CHECK_RETURN(Engine::Ready_PhysicsMgr(1), E_FAIL);

	return S_OK;
}

void CMainApp::CleanupDeviceD3D()
{
	if (m_pGraphicDev) { m_pGraphicDev->Release(); m_pGraphicDev = nullptr; }
	if (m_pSDK) { m_pSDK->Release(); m_pSDK = nullptr; }
}

void CMainApp::ResetDevice(_uint dwResizeWidth, _uint dwResizeHeight)
{
	CImguiMgr::GetInstance()->ResetDevice(dwResizeWidth, dwResizeHeight);
}