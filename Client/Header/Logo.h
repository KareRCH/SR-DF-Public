#pragma once

#include "Scene.h"

#include "BackGround.h"
#include "Loading.h"

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

	virtual HRESULT		ReadyLate_Scene();

private:
	HRESULT				Ready_Texture();
	HRESULT				Ready_Prototype();
	HRESULT				Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag) { return S_OK; }
	HRESULT				Ready_Layer_UI(const _tchar* pLayerTag) { return S_OK; }

private:
	DELAY<float>		m_fSkipStartFrame = DELAY<float>(0.1f);
	_bool				m_bInitFrame;
	CLoading*			m_pLoading;
	HRESULT				m_hrLoading;

protected:
	virtual HRESULT		Ready_Layer_Completed() { return S_OK; }

};

