#pragma once
#include "AceEffect.h"

class CEffect_CartridgeCase : public CAceEffect
{
	DERIVED_CLASS(CAceEffect, CEffect_CartridgeCase)

private:
	explicit CEffect_CartridgeCase(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_CartridgeCase(const CEffect_CartridgeCase& rhs);
	virtual ~CEffect_CartridgeCase();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEffect_CartridgeCase* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, _vec3 vRight, CGameObject* pOwner);

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
	_vec3 PlayerRight;
};


