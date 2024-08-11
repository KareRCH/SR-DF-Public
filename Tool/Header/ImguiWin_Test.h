#pragma once

#include "ImguiWin.h"

class CImguiWin_Test : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_Test)

private:
	explicit CImguiWin_Test();
	virtual ~CImguiWin_Test();

public:
	static CImguiWin_Test* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

public:
	GETSET_EX1(LPDIRECT3DTEXTURE9, m_pTexture, Texture, GET_REF)

private:
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface = nullptr;


};

