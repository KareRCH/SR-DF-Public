#include "TimerMgr.h"

IMPLEMENT_SINGLETON(CTimerMgr)

CTimerMgr::CTimerMgr()
{
}

CTimerMgr::~CTimerMgr()
{
	Free();
}

_float CTimerMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.f);

	return pTimer->Get_TimeDelta();
}

void CTimerMgr::Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	NULL_CHECK(pTimer);

	pTimer->SetUp_TimeDelta();
}

HRESULT CTimerMgr::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

CTimer* CTimerMgr::Find_Timer(const _tchar* pTimerTag) const
{
	auto	iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

void CTimerMgr::Free()
{
	for_each(m_mapTimers.begin(), m_mapTimers.end(), CDeleteMap());
	m_mapTimers.clear();
}
