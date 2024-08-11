#pragma once

#include "Base.h"
#include "Engine_Define.h"

class ENGINE_DLL CImguiMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CImguiMgr)

private:
	explicit CImguiMgr();
	virtual ~CImguiMgr();

public:
	virtual void	Free();

public:
	HRESULT Ready_Imgui();
	HRESULT Update_Imgui();
	HRESULT Render_Imgui();
};

