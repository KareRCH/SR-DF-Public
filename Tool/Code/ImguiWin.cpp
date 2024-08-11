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
	// �ڽ� ������ ������ �������� ����.
	if (m_vecChildWindows.size() <= 0)
		return 0;

	// �켱���� ���� ����, �켱���� Ŭ���� ������ �´�.
	sort(m_vecChildWindows.begin(), m_vecChildWindows.end(), 
		[](CImguiWin*& pDst, CImguiWin*& pSrc) {
		return (pDst->Get_Priority() < pSrc->Get_Priority());
		});

	// �ڽ� ������ ������
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
