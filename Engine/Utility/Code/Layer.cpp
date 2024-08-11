#include "Layer.h"

CLayer::CLayer()
	: m_fPriority(0.f)
{

}

CLayer::~CLayer()
{

}

CLayer* CLayer::Create(_float fPriority)
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer(fPriority)))
	{
		Safe_Release(pLayer);

		MSG_BOX("Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	// 오브젝트 컨테이너를 비우며, 오브젝트를 삭제한다.
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}

HRESULT CLayer::Ready_Layer(_float fPriority)
{
	m_fPriority = fPriority;

	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{
	// Set Dead로 설정된 객체를 삭제시킨다.
	for (auto iter = m_mapObject.begin(); iter != m_mapObject.end();)
	{
		if ((*iter).second->Get_IsDead())
		{
			// 레퍼런스 카운터가 1이상이었을 때 추가적인 해제 작업이 필요하다.
			_uint iRefCount = Safe_Release((*iter).second);
			iter = m_mapObject.erase(iter);
		}
		else
			++iter;
	}

	// 우선도 설정을 해준다.
	for (_uint i = 0; i < static_cast<_uint>(EPRIORITY::RENDER); i++)
	{
		m_arrvecPriorityObject[i].reserve(m_mapObject.size());

		// vector에 객체 추가
		for (auto& item : m_mapObject)
			m_arrvecPriorityObject[i].push_back(item.second);

		// 우선도 기반 정렬
		sort(m_arrvecPriorityObject[i].begin(), m_arrvecPriorityObject[i].end(),
			[&i](CGameObject* const pDst, CGameObject* const pSrc) {
				if (!pDst->Get_IsUsePriority(i) && pDst->Get_IsUsePriority(i))
					return true;
				else if (!pDst->Get_IsUsePriority(i) && !pDst->Get_IsUsePriority(i))
					return false;
				return (pDst->Get_Priority(i) > pSrc->Get_Priority(i));
			});
	}

	// 이 레이어에 속한 게임 오브젝트를 업데이트 한다.
	_int	iResult = 0;
	for (auto& gameObj : m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::UPDATE)])
	{
		iResult = gameObj->Update_GameObject(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	// 쓰고 나면 클리어
	m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::UPDATE)].clear();

	return iResult;
}

void CLayer::LateUpdate_Layer()
{
	for (auto& gameObj : m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::LATE)])
	{
		gameObj->LateUpdate_GameObject();
	}

	// 쓰고 나면 클리어
	m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::LATE)].clear();
}

CComponent* CLayer::Get_Component(COMPONENTID eID, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	// 레이어 -> 오브젝트 -> 컴포넌트
	//auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));
	auto iter = m_mapObject.find(pObjTag);

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second->Get_Component(eID, pComponentTag);
}

// 인게임에서 추가할 때 (Update 타이밍때)
HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	// 이 레이어에 게임 오브젝트를 적재한다.
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	wstring strObjectTag = pGameObject->Get_ObjectName();
	auto iter = m_mapObject.find(strObjectTag.c_str());
	// 대충 중복 이름 있으면 숫자 붙여서 넣는 방식
	if (iter != m_mapObject.end())
	{
		_uint i = 0U;
		while (true)
		{
			wstringstream ss;
			ss << i;
			wstring strNew = strObjectTag + ss.str();
			auto iterObject = m_mapObject.find(strNew);
			// 없으면 추가하기
			if (iterObject == m_mapObject.end())
			{
				m_mapObject.emplace(strNew, pGameObject);
				pGameObject->Set_ObjectName(strNew);
				break;
			}

			++i;
		}

	}
	else
		m_mapObject.emplace(strObjectTag.c_str(), pGameObject);

	return S_OK;
}

// 씬 레디 전용
HRESULT CLayer::Add_GameObject(const wstring pObjTag, CGameObject* pGameObject)
{
	// 이 레이어에 게임 오브젝트를 적재한다.
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	auto iter = m_mapObject.find(pObjTag);
	// 대충 중복 이름 있으면 숫자 붙여서 넣는 방식
	if (iter != m_mapObject.end())
	{
		_uint i = 0U;
		while (true)
		{
			wstringstream ss;
			ss << i;
			wstring strNew = pObjTag + ss.str();
			auto iterObject = m_mapObject.find(strNew);
			// 없으면 추가하기
			if (iterObject == m_mapObject.end())
			{
				m_mapObject.emplace(strNew, pGameObject);
				pGameObject->Set_ObjectName(strNew);
				break;
			}

			++i;
		}
		
	}
	else
	{
		m_mapObject.emplace(pObjTag, pGameObject);
		pGameObject->Set_ObjectName(pObjTag);
	}
		

	return S_OK;
}

CGameObject* CLayer::Get_GameObject(const wstring pObjTag)
{
	auto iter = m_mapObject.find(pObjTag);
	if (iter != m_mapObject.end())
		return (*iter).second;

	return nullptr;
}
