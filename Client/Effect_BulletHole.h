#pragma once
#include "AceEffect.h"

class CEffect_BulletHole : public CAceEffect
{
	DERIVED_CLASS(CAceEffect, CEffect_BulletHole)

private:
	explicit CEffect_BulletHole(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_BulletHole(const CEffect_BulletHole& rhs);
	virtual ~CEffect_BulletHole();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEffect_BulletHole* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y,
		_float _z, STATE_RIGHTHAND pHand);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

public:
	STATE_RIGHTHAND GunType;
};


