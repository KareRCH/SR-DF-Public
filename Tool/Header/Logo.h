#pragma once

#include "Scene.h"

class CLogo : public Engine::CScene
{
	DERIVED_CLASS(CScene, CLogo)
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	static CLogo*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

private:
	HRESULT				Ready_Prototype();
	HRESULT				Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag) { return S_OK; }
	HRESULT				Ready_Layer_UI(const _tchar* pLayerTag) { return S_OK; }

protected:
	virtual HRESULT		Ready_Layer_Completed() { return S_OK; }

};

