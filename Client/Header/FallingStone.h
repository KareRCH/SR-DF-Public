#pragma once
#include "MonsterAttackUnion.h"

class CFallingStone : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CFallingStone)

private:
	explicit CFallingStone(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFallingStone(const CFallingStone& rhs);
	virtual ~CFallingStone();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CFallingStone* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _pHASE, CAceUnit* pOwner, ETEAM_ID _eTeamid);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	
		// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionSphere* pShape;

private:
	_vec3 vFall;				// 떨어지는 방향 
	_float m_fFallingSpeed;		// 떨어지는 스피드 
	_bool m_bCollision = FALSE; //충돌해서 시간흐르는용도 
	_bool m_bFall = TRUE;		// 어딘가에서 충돌이 이루어지면 깨지는 이미지로 바뀌고 LifeTime이 지나면 Dead()
	void Falling(const _float& fTimeDelta); // 떨어지는 함수 
	_bool m_bSound = TRUE;
};

