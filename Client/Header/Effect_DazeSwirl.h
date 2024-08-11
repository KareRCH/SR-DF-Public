#pragma once
#include "AceEffect.h"

class CEffect_DazeSwirl : public CAceEffect
{
	DERIVED_CLASS(CAceEffect, CEffect_DazeSwirl)

private:
	explicit CEffect_DazeSwirl(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_DazeSwirl(const CEffect_DazeSwirl& rhs);
	virtual ~CEffect_DazeSwirl();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEffect_DazeSwirl* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

};


