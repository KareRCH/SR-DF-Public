#pragma once
#include "AceEffect.h"

class CEffect_FallingDust : public CAceEffect
{
	DERIVED_CLASS(CAceEffect, CEffect_FallingDust)

private:
	explicit CEffect_FallingDust(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_FallingDust(const CEffect_FallingDust& rhs);
	virtual ~CEffect_FallingDust();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEffect_FallingDust* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

public:
	HRESULT Update_Pos();
	_vec3 OwnerPos;
	_vec3 EffectPos;
	_vec3 ResultDir;

};


