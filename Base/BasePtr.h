#pragma once

class CBase;

class _declspec(dllexport) CBasePtr
{
protected:
	inline explicit CBasePtr();
	inline virtual ~CBasePtr();

public:
	inline CBase*				Get_Base();
	inline unsigned		long	Get_RefCount();

private:
	CBase* m_pBase;

public:

};

#include "BasePtr.inl"