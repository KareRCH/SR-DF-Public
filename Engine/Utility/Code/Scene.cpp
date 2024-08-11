#include "Scene.h"



CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
}

void CScene::Free()
{
	// 레이어 컨테이너 비우고 삭제
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	// 레퍼런스 카운트 줄이기
	Safe_Release(m_pGraphicDev);
}

HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
	// 우선도 설정을 해준다.
	{
		m_vecPriorityLayer.reserve(m_mapLayer.size());

		// vector에 객체 추가
		for (auto& item : m_mapLayer)
			m_vecPriorityLayer.push_back(item.second);

		// 우선도 기반 정렬
		sort(m_vecPriorityLayer.begin(), m_vecPriorityLayer.end(),
			[](CLayer* const pDst, CLayer* const pSrc) {
				return pDst->Get_Priority() > pSrc->Get_Priority();
			});
	}

	_int	iResult = 0;
	for (auto& iter : m_vecPriorityLayer)
	{
		iResult = iter->Update_Layer(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	return iResult;
}

void CScene::LateUpdate_Scene()
{
	for (auto& iter : m_vecPriorityLayer)
		iter->LateUpdate_Layer();

	// 우선도 벡터 초기화
	m_vecPriorityLayer.clear();
}

void CScene::Render_Scene()
{
	// _DEBUG 용

}

HRESULT CScene::ReadyLate_Scene()
{
	return S_OK;
}

CComponent* CScene::Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	// 씬 -> 레이어 -> 오브젝트 -> 컴포넌트
	//auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));
	auto iter = m_mapLayer.find(pLayerTag);

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(eID, pObjTag, pComponentTag);
}

CGameObject* CScene::Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	CLayer* pLayer = m_mapLayer[pLayerTag];
	NULL_CHECK_RETURN(pLayer, nullptr);

	CGameObject* pObj = pLayer->Get_GameObject(pObjTag);

	return pObj;
}

void CScene::Add_GameObject(const _tchar* pLayerTag, CGameObject* pObj)
{
	auto iter = m_mapLayer.find(pLayerTag);
	if (iter == m_mapLayer.end())
	{
		OutputDebugString(L"레이어 없음!");
		Safe_Release(pObj);
		return;
	}

	(*iter).second->Add_GameObject(pObj);
}

void CScene::Add_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag, CGameObject* pObj)
{
	auto iter = m_mapLayer.find(pLayerTag);
	if (iter == m_mapLayer.end())
	{
		OutputDebugString(L"레이어 없음!");
		Safe_Release(pObj);
		return;
	}

	(*iter).second->Add_GameObject(pObjTag, pObj);
}

HRESULT CScene::Add_Layer(const _tchar* pLayerTag, CLayer* pLayer)
{
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CScene::Delete_LayerAll()
{
	for (auto iter = m_mapLayer.begin(); iter != m_mapLayer.end();)
	{
		Safe_Release((*iter).second);
		iter = m_mapLayer.erase(iter);
	}
	

	return S_OK;
}
