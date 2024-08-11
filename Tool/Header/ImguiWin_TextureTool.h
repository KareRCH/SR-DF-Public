#pragma once

#include "ImguiWin.h"

class CImguiWin_TextureTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_TextureTool)

private:
	explicit CImguiWin_TextureTool();
	virtual ~CImguiWin_TextureTool();

public:
	static CImguiWin_TextureTool* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;
};

