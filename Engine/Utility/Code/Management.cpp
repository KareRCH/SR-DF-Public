#include "Management.h"

#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pScene_Current(nullptr)
{
}

CManagement::~CManagement()
{
	Free();
}

void CManagement::Free()
{
	switch (m_eSceneProcess)
	{
	case EMANAGE_SCENE::SINGLE:
		Safe_Release(m_pScene_Current);
		break;
	case EMANAGE_SCENE::MULTI:
		for (auto& item : m_mapScene)
			Safe_Release(item.second);
		m_mapScene.clear();
		break;
	}
}

CComponent* Engine::CManagement::Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	// 매니지먼트 -> 씬 -> 레이어 -> 게임 오브젝트 -> 컴포넌트
	if (nullptr == m_pScene_Current)
		return nullptr;

	return m_pScene_Current->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}

CGameObject* CManagement::Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	// 매니지먼트 -> 씬 -> 레이어 -> 게임 오브젝트
	if (nullptr == m_pScene_Current)
		return nullptr;

	return m_pScene_Current->Get_GameObject(pLayerTag, pObjTag);
}

void CManagement::Add_GameObject(const _tchar* pLayerTag, CGameObject* const pObj)
{
	m_pScene_Current->Add_GameObject(pLayerTag, pObj);
}

void CManagement::Add_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag, CGameObject* const pObj)
{
	m_pScene_Current->Add_GameObject(pLayerTag, pObjTag, pObj);
}

void CManagement::Add_Layer(const _tchar* pLayerTag, CLayer* const pLayer)
{
	m_pScene_Current->Add_Layer(pLayerTag, pLayer);
}

HRESULT CManagement::Ready_Management(const EMANAGE_SCENE eType)
{
	m_eSceneProcess = eType;

	return S_OK;
}

_int CManagement::Update_Scene(const _float& fTimeDelta)
{
	if (m_pScene_Reserve)
	{
		switch (m_eSceneProcess)
		{
		case Engine::EMANAGE_SCENE::SINGLE:
		{
			Safe_Release(m_pScene_Current);
			m_pScene_Current = m_pScene_Reserve;
			m_pScene_Reserve = nullptr;
			break;
		}
		case Engine::EMANAGE_SCENE::MULTI:
		{
			m_pScene_Current = m_pScene_Reserve;
			m_pScene_Reserve = nullptr;
			break;
		}
		}

		Engine::Clear_RenderGroup();
		Engine::Play_PhysicsSimulation(0);
	}

	// 씬 없으면 예외처리
	NULL_CHECK_RETURN(m_pScene_Current, -1)

	return m_pScene_Current->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene()
{
	// 씬 없으면 예외처리
	NULL_CHECK(m_pScene_Current);
	m_pScene_Current->LateUpdate_Scene();
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::Render_GameObject(pGraphicDev);

	NULL_CHECK(m_pScene_Current);
	m_pScene_Current->Render_Scene();		// 디버깅용
}

HRESULT CManagement::Set_Scene(CScene* pScene)
{
	if (m_eSceneProcess != EMANAGE_SCENE::SINGLE)
		return E_FAIL;

	// 씬 해제 후 새로운 씬을 로드
	m_pScene_Reserve = pScene;
	Engine::Stop_PhysicsSimulation(0);

	return S_OK;
}

HRESULT CManagement::Set_Scene(wstring strSceneName)
{
	auto iter = m_mapScene.find(strSceneName);
	if (iter == m_mapScene.end())
		return E_FAIL;

	switch (m_eSceneProcess)
	{
	case EMANAGE_SCENE::SINGLE:
		// 씬 해제 후 새로운 씬을 로드
		m_pScene_Reserve = (*iter).second;

		break;
	case EMANAGE_SCENE::MULTI:
		// 기존 씬 자체는 유지한다. 씬을 로드하러 올 때 그 상태 그대로 로드한다.
		// + 나중에 씬이 멈췄음을 인지할 수 있는 이벤트를 만들어 넣어주어야 한다.
		// 이를 통해 씬과 관련없는 매니저와 같은 곳에서 로드되지 않은 자원을 쓰는 객체에 대해 통제할 수 있다.
		// 특히 물리엔진
		m_pScene_Reserve = (*iter).second;

		break;
	}
	Engine::Stop_PhysicsSimulation(0);

	return S_OK;
}

HRESULT CManagement::Add_Scene(CScene* pScene, wstring strSceneName)
{
	m_mapScene.emplace(strSceneName, pScene);

	return S_OK;
}

HRESULT CManagement::Clear_CurrentScene()
{
	m_pScene_Current->Delete_LayerAll();

	return S_OK;
}

HRESULT CManagement::Clear_SceneAll()
{
	for (auto& item : m_mapScene)
		item.second->Delete_LayerAll();

	return S_OK;
}

HRESULT CManagement::Delete_SceneAll()
{
	for (auto& item : m_mapScene)
		Safe_Release(item.second);

	return S_OK;
}

HRESULT CManagement::Delete_CurrentScene()
{
	Safe_Release(m_pScene_Current);

	return S_OK;
}
