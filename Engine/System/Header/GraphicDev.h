#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDev : public CBase
{
	DECLARE_SINGLETON(CGraphicDev)
public:
	explicit CGraphicDev();
	virtual ~CGraphicDev();

public:
	virtual void	Free();

public:
	HRESULT		Get_DeviceReadyState() { return m_dwReady; }

public:
	HRESULT		Ready_GraphicDev(CGraphicDev** ppGraphicClass, HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY);
	HRESULT		Reset_GraphicDev();
	HRESULT		Render_Begin(D3DXCOLOR Color);
	HRESULT		Render_End();

public:
	GETSET_EX1(LPDIRECT3D9,				m_pSDK, GraphicSDK,			GET)
	GETSET_EX1(LPDIRECT3DDEVICE9,		m_pGraphicDev, GraphicDev,	GET)
	GETSET_EX1(D3DPRESENT_PARAMETERS,	m_d3dpp, D3DPP,				GET_PTR)
	GETSET_EX1(LPD3DXSPRITE,			m_pSprite, Sprite,			GET)
	GETSET_EX1(LPD3DXFONT,				m_pFont, Font,				GET)

private:
	LPDIRECT3D9				m_pSDK;
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	D3DPRESENT_PARAMETERS	m_d3dpp;
	LPD3DXSPRITE			m_pSprite;
	LPD3DXFONT				m_pFont;

//public:
//	GETSET_EX1(HRESULT,					m_dwResult, Result, GET)

private:
	HRESULT					m_dwReady = E_FAIL;
};

END