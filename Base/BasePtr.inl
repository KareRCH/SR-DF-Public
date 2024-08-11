#include "Base.h"

CBasePtr::CBasePtr()
	: m_pBase(nullptr)
{

}
CBasePtr::~CBasePtr()
{

}

CBase* CBasePtr::Get_Base()
{
	if (m_pBase)
	{

	}

	return ++m_dwRefCnt;
}
unsigned	long		CBasePtr::Get_RefCount()
{
	return m_dwRefCnt;
}