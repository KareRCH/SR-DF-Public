#include "stdafx.h"
#include "MainApp.h"

#include "Logo.h"

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
	ThisClass* pInstance = new ThisClass;

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
	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	return S_OK;
}

int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	// 다이렉트 인풋 업데이트, 
	Engine::Update_KeyMgr();

	// 물리 업데이트
	Engine::StartFrame_PhysicsMgr();
	// 씬 업데이트
	m_pManagementClass->Update_Scene(fTimeDelta);
	auto start = chrono::high_resolution_clock::now();

	// 물리 업데이트
	Engine::Update_PhysicsMgr(fTimeDelta);

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> duration = end - start;

	

	

	cout << "업데이트 실행시간 : " << duration.count() << endl;

	return 0;
}

void CMainApp::LateUpdate_MainApp()
{
	Engine::LateUpdate_Scene();
	Engine::LateUpdate_KeyMgr();		// 가상키 시스템 업데이트, 키 상태 변경 단계
}

void CMainApp::Render_MainApp()
{
	Engine::Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));


	//auto start = chrono::high_resolution_clock::now();

	Engine::Render_Scene(m_pGraphicDev);

	/*auto end = chrono::high_resolution_clock::now();
	chrono::duration<double> duration = end - start;

	cout << "렌더 실행시간 : " << duration.count() << endl;*/

	Render_FrameRate();

	Engine::Render_End();
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement)
{
	Engine::CScene* pScene = nullptr;

	
	// 씬 매니지먼트 추가
	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagement, EMANAGE_SCENE::SINGLE), E_FAIL);
	(*ppManagement)->AddRef();

	// 기본 씬
	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);

	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(pScene), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	// 장치 초기화
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
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Thin_Jinji", L"궁서", 18, 30, FW_THIN), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"MonsterUI", L"함초롱바탕", 14, 25, FW_THIN), E_FAIL);

	// Renderer
	FAILED_CHECK_RETURN(Engine::Ready_Renderer(), E_FAIL);

	// TextureMgr
	FAILED_CHECK_RETURN(Engine::Ready_TextureMgr(*ppGraphicDev), E_FAIL);

	// BlackBoardMgr
	FAILED_CHECK_RETURN(Engine::Ready_BlackBoardMgr(), E_FAIL);

	// PhysicsMgr
	FAILED_CHECK_RETURN(Engine::Ready_PhysicsMgr(1), E_FAIL);

	// 팀 에이전트 팀 설정, A, B팀 서로 적대 관계 설정 - NONE,	HOSTILE(적대),	NUETRAL(중립),		FRIEND(호감) 
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_PLAYER), static_cast<_int>(ETEAM_MONSTER), ERELATION_STATE::HOSTILE); //플레이어가 몬스터에게 적대관계 
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_PLAYER), static_cast<_int>(ETEAM_BOSS), ERELATION_STATE::HOSTILE);		//플레이어가 보스에게 적대관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_PLAYER), static_cast<_int>(ETEAM_OBJECT), ERELATION_STATE::NUETRAL);	//플레이어가 오브젝트에 중립관계

	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_MONSTER), static_cast<_int>(ETEAM_BOSS), ERELATION_STATE::FRIEND);		// 몬스터가 보스에게 친밀관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_MONSTER), static_cast<_int>(ETEAM_PLAYER), ERELATION_STATE::HOSTILE);	// 몬스터가 플레이어에게 적대관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_MONSTER), static_cast<_int>(ETEAM_OBJECT), ERELATION_STATE::NUETRAL);	// 몬스터가 오브젝트에 중립관계

	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_BOSS), static_cast<_int>(ETEAM_PLAYER), ERELATION_STATE::HOSTILE);		// 보스가 플레이어에게 적대관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_BOSS), static_cast<_int>(ETEAM_OBJECT), ERELATION_STATE::NUETRAL);		// 보스가 오브젝트에 중립관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_BOSS), static_cast<_int>(ETEAM_MONSTER), ERELATION_STATE::HOSTILE);	// 보스가 몬스터에게 적대관계

	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_OBJECT), static_cast<_int>(ETEAM_PLAYER), ERELATION_STATE::NUETRAL);	// 오브젝트가 플레이어에게 중립관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_OBJECT), static_cast<_int>(ETEAM_MONSTER), ERELATION_STATE::NUETRAL);	// 오브젝트가 몬스터에게 중립관계
	ITeamAgent::Add_TeamRelation(static_cast<_int>(ETEAM_OBJECT), static_cast<_int>(ETEAM_BOSS), ERELATION_STATE::NUETRAL);		// 오브젝트가 보스에게 중립관계
	

	return S_OK;
}

void CMainApp::Render_FrameRate()
{
#ifndef _DEBUG
	return;
#endif

	_float fFrameRate = Engine::Get_FrameRate(L"Frame");
	_float fTimeDelta = Engine::Get_TimeDelta(L"Timer_FPS");
	_float fResultFrameRate = static_cast<_float>(fFrameRate / (fTimeDelta * fFrameRate));
	
	wstringstream ss;
	ss << fResultFrameRate;
	wstring str = ss.str();
	_vec2 vFontPos = { 0.f, 0.f };
	D3DXCOLOR colorFont;
	
	if (fResultFrameRate >= fFrameRate * 0.9f)
		colorFont = DXCOLOR_GREEN;
	else if (fResultFrameRate >= fFrameRate * 0.5f)
		colorFont = DXCOLOR_ORANGE;
	else
		colorFont = DXCOLOR_RED;

	Engine::Render_Font(L"Font_Jinji", str.c_str(), &vFontPos, colorFont);
}
