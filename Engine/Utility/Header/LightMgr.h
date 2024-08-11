#pragma once

#include "Light.h"

class ENGINE_DLL CLightMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CLightMgr)

private:
	explicit CLightMgr();
	virtual ~CLightMgr();

private:
	virtual		void	Free();

public:
	HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
								const D3DLIGHT9* pLightInfo,
								const _uint& iIndex);

private:
	list<CLight*>			m_LightList;


};

