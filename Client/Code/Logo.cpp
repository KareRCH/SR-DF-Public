#include "stdafx.h"
#include "Logo.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Scene_Parsed.h"
#include <Stage.h>
#include <DynamicCamera.h>
#include <TitleLogo.h>

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CLogo::Ready_Scene()
{

	g_bLockEsc = true;
	m_bInitFrame = true;

	FAILED_CHECK_RETURN(Ready_Texture(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = SUPER::Update_Scene(fTimeDelta);

	// m_pLoading은 m_bInitFrame에 종속적입니다.
	if (m_fSkipStartFrame.Update(fTimeDelta))
	{
		if (m_bInitFrame)
		{
			// 로딩 쓰레드 생성하기
			m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
			if (m_pLoading == nullptr)
				m_hrLoading = E_FAIL;
			else
			{
				m_hrLoading = S_OK;
				m_bInitFrame = false;
			}
		}
	}

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	SUPER::LateUpdate_Scene();


	// 로딩이 끝났을 때 엔터를 눌러 스테이지 씬으로 진입할 수 있다.
	if (!m_bInitFrame && m_pLoading->Get_Finish())
	{
		g_bLockEsc = false;

		if (Engine::IsKey_Pressed(DIK_RETURN))
		{
			//CScene* pScene = CStage::Create(m_pGraphicDev);
			CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "Stage1");
			//CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "BossStage");
			//CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "Malone");
			//CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "SeongHee");
			//CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "TestPhysics");

			/*주의
			* 마법의 병은 Malone씬에서 넘어갈떄 트리거에 의해 작동하므로 그냥 성희맵으로 들어가면 마법의 병 적용x
			*/

			NULL_CHECK(pScene);

			Engine::Set_Scene(pScene);
		}
	}
}

void CLogo::Render_Scene()
{
	// _DEBUG 용
}

HRESULT CLogo::ReadyLate_Scene()
{
	return S_OK;
}

HRESULT CLogo::Ready_Texture()
{
	// 로고용 텍스처들
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/LogoShot.png", TEX_NORMAL, L"UI", L"Logo"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/Title.png", TEX_NORMAL, L"UI", L"Title"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"Resource/Texture/Scene/Title_%d.png", TEX_NORMAL, L"UI", L"Title", _range<_uint>(0U, 2U), E_FAIL);

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
	// 프로토타입 인스턴스를 등록한다.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// 트랜스폼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// 버퍼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// 텍스처
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Title")), E_FAIL);	// 텍스처
	
	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// 정상
	// 레이어 초기화
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create(0.f)), E_FAIL);

	// 배경생성
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(CBackGround::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	SUPER::Free();
}

