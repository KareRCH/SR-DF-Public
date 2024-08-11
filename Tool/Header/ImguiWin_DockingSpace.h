#pragma once

#include "ImguiWin.h"

class CImguiWin_DockingSpace : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_DockingSpace)

private:
	explicit CImguiWin_DockingSpace();
	virtual ~CImguiWin_DockingSpace();

public:
	static CImguiWin_DockingSpace* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;


private:
	_bool					m_bOpt_FullScreen;
	_bool					m_bOpt_Padding;

	ImGuiDockNodeFlags		m_DockSpace_Flags;
	ImGuiWindowFlags		m_Window_Flags;

	


};

