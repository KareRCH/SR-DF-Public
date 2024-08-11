#pragma once
#include "MonsterAttackUnion.h"


class CRedLaser : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CRedLaser)

private:
	explicit CRedLaser(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRedLaser(const CRedLaser& rhs);
	virtual ~CRedLaser();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CRedLaser* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								_float _x, _float _y, _float _z, CAceUnit* pOwner, ETEAM_ID _eTeamid);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

	// 충돌 
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;

	//변수
private:
	_bool KnockBack = FALSE;
};

