
#pragma once
#include "MonsterAttackUnion.h"

class CEnergyBall : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CEnergyBall)

private:
	explicit CEnergyBall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEnergyBall(const CEnergyBall& rhs);
	virtual ~CEnergyBall();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEnergyBall* Create(LPDIRECT3DDEVICE9 pGraphicDev,
								_float _x, _float _y, _float _z, MonsterPhase _CurrPhase, CGameObject* pOwner, ETEAM_ID _eTeamid);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// Ãæµ¹ 
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionSphere* pShape;

private:
	void Value_Setting(_float _x, _float _y, _float _z, MonsterPhase _phase);
	HRESULT Follow_Player(const _float fTimeDelta);

	_float m_fMovingSpeed; 
	_float m_fGuideTime;
	_vec3 Dir;
};


