#pragma once

#include "Scene.h"

class CScene_Parsed : public CScene
{
	DERIVED_CLASS(CScene, CScene_Parsed)

private:
	explicit CScene_Parsed(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Parsed();

public:
	static CScene_Parsed* Create(LPDIRECT3DDEVICE9 pGraphicDev, const char* pSceneFileName);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual HRESULT		Ready_Scene(const char* pSceneFileName);
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

	virtual HRESULT		ReadyLate_Scene();

protected:
	HRESULT				Ready_Layer();
	virtual HRESULT		Ready_Layer_Completed();
};

