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

	// m_pLoading�� m_bInitFrame�� �������Դϴ�.
	if (m_fSkipStartFrame.Update(fTimeDelta))
	{
		if (m_bInitFrame)
		{
			// �ε� ������ �����ϱ�
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


	// �ε��� ������ �� ���͸� ���� �������� ������ ������ �� �ִ�.
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

			/*����
			* ������ ���� Malone������ �Ѿ�� Ʈ���ſ� ���� �۵��ϹǷ� �׳� ��������� ���� ������ �� ����x
			*/

			NULL_CHECK(pScene);

			Engine::Set_Scene(pScene);
		}
	}
}

void CLogo::Render_Scene()
{
	// _DEBUG ��
}

HRESULT CLogo::ReadyLate_Scene()
{
	return S_OK;
}

HRESULT CLogo::Ready_Texture()
{
	// �ΰ�� �ؽ�ó��
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/LogoShot.png", TEX_NORMAL, L"UI", L"Logo"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/Title.png", TEX_NORMAL, L"UI", L"Title"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"Resource/Texture/Scene/Title_%d.png", TEX_NORMAL, L"UI", L"Title", _range<_uint>(0U, 2U), E_FAIL);

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
	// ������Ÿ�� �ν��Ͻ��� ����Ѵ�.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// Ʈ������
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// ����
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// �ؽ�ó
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Title")), E_FAIL);	// �ؽ�ó
	
	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// ����
	// ���̾� �ʱ�ȭ
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create(0.f)), E_FAIL);

	// ������
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(CBackGround::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	SUPER::Free();
}

