#pragma once

#include "Scene.h"

class CScene_AnimationTool : public Engine::CScene
{
	DERIVED_CLASS(CScene, CScene_AnimationTool)
private:
	explicit CScene_AnimationTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_AnimationTool();

public:
	static CScene_AnimationTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

	

protected:
	HRESULT Ready_Prototype();
	virtual HRESULT		Ready_Layer_Completed() { return S_OK; }

private:
	//HRESULT				Ready_LightInfo();
	//HRESULT				Ready_SpotLightInfo();

	HRESULT					Ready_Layer_Environment(const _tchar* pLayerTag);
	//HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag);
	//HRESULT				Ready_Layer_Camera(const _tchar* pLayerTag);
	//HRESULT				Ready_Layer_UI(const _tchar* pLayerTag);


};

