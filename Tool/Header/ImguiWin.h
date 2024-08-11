#pragma once

#include "Base.h"
#include "Engine_Define.h"

/// <summary>
/// IMGUI를 기반으로 설계된 윈도우 단위의 클래스
/// </summary>
class CImguiWin abstract : public CBase
{
	DERIVED_CLASS(CBase, CImguiWin)

public:
	explicit CImguiWin();
	virtual ~CImguiWin();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() PURE;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta);
	// IMGUI는 자동으로 렌더를 돌리기 때문에 따로 함수가 필요없다.

public:
	GETSET_EX1(vector<CImguiWin*>, m_vecChildWindows, ChildWindows, GET_C_REF)
	CImguiWin*	Get_ChildWindow(const _uint value) { return m_vecChildWindows[value]; }
	void		Add_ChildWindow(CImguiWin* pChildWin);

	GETSET_EX2(_float, m_fPriority, Priority, GET_C, SET)

protected:
	vector<CImguiWin*>	m_vecChildWindows;
	_float				m_fPriority;


public:
	GETSET_EX1(_bool, m_bOpen, Open, SET)
	GETSET_EX1(_bool, m_bFirstLoop, FirstLoop, SET)

protected:	// 각종 속성
	_bool					m_bOpen;
	_bool					m_bFirstLoop;
};

