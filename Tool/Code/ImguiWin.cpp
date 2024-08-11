#include "ImguiWin.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

CImguiWin::CImguiWin()
	: m_fPriority(0.f), m_bFirstLoop(true)
{
}

CImguiWin::~CImguiWin()
{
}

void CImguiWin::Free()
{
	
}

_int CImguiWin::Update_ImguiWin(const _float& fTimeDelta)
{
	// 자식 윈도우 없으면 실행하지 않음.
	if (m_vecChildWindows.size() <= 0)
		return 0;

	// 우선도에 따른 정렬, 우선도가 클수록 앞으로 온다.
	sort(m_vecChildWindows.begin(), m_vecChildWindows.end(), 
		[](CImguiWin*& pDst, CImguiWin*& pSrc) {
		return (pDst->Get_Priority() < pSrc->Get_Priority());
		});

	// 자식 윈도우 돌리기
	for (auto item : m_vecChildWindows)
	{
		item->Update_ImguiWin(fTimeDelta);
	}

	return 0;
}

void CImguiWin::Add_ChildWindow(CImguiWin* pChildWin)
{
	if (m_vecChildWindows.capacity() <= 0)
		m_vecChildWindows.reserve(2);

	m_vecChildWindows.push_back(pChildWin);
}
