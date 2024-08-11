#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer();

public:
	_float Get_TimeDelta() { return m_fTimeDelta; }
	HRESULT	Ready_Timer();
	void	SetUp_TimeDelta();

private:
	LARGE_INTEGER		m_FrameTime;
	LARGE_INTEGER		m_FixTime;
	LARGE_INTEGER		m_LastTime;
	LARGE_INTEGER		m_CpuTick;

	_float			m_fTimeDelta;

public:
	static CTimer* Create();

private:
	virtual void			Free();
};

END