#pragma once

#include "Scene.h"

class CScene_TextureTool : public Engine::CScene
{
	DERIVED_CLASS(CScene, CScene_TextureTool)
private:
	explicit CScene_TextureTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_TextureTool();

public:
	static CScene_TextureTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

protected:
	virtual HRESULT		Ready_Layer_Completed();


};

