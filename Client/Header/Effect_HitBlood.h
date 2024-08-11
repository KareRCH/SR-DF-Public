#pragma once
#include "AceEffect.h"

class CEffect_HitBlood : public CAceEffect
{
	DERIVED_CLASS(CAceEffect, CEffect_HitBlood)

private:
	explicit CEffect_HitBlood(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_HitBlood(const CEffect_HitBlood& rhs);
	virtual ~CEffect_HitBlood();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEffect_HitBlood* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									_float _x, _float _y, _float _z,
									CGameObject* pOwner);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

public:
	_vec3 m_fVelocity;
	_vec3 m_fAcceleration;
	_bool m_bTrigger;

};