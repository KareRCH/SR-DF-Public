#pragma once
#include "MonsterAttackUnion.h"

class CRedThunder : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CRedThunder)

private:
	explicit CRedThunder(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRedThunder(const CRedThunder& rhs);
	virtual ~CRedThunder();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CRedThunder* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		_float _x, _float _y, _float _z, MonsterPhase _CurrPhase, CGameObject* pOwner, ETEAM_ID _eTeamid);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	FCollisionBox* pBoxShape;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// Ãæµ¹ -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;

private:
};
