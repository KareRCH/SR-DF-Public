#pragma once

#include "Scene.h"


/// <summary>
/// 프로토 툴 전용 씬
/// </summary>
class CScene_ProtoTool : public CScene
{
	DERIVED_CLASS(CScene, CScene_ProtoTool)
private:
	explicit CScene_ProtoTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_ProtoTool();

public:
	static CScene_ProtoTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
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

