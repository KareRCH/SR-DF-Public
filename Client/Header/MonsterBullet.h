#pragma once
#include "MonsterAttackUnion.h"

class CMonsterBullet : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CMonsterBullet)

	PUBLIC enum class TYPE { NORMAL, HEAVY, TYPE_END };

private:
	explicit CMonsterBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonsterBullet(const CMonsterBullet& rhs);
	virtual ~CMonsterBullet();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CMonsterBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev,
								_float _x, _float _y, _float _z,
								TYPE _option, CAceUnit* pOwner, ETEAM_ID _eTeamid);

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
	virtual void	OnCollision(CGameObject* pDst, const FContact* const);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const);
	virtual void	OnCollisionExited(CGameObject* pDst);

private:
	void					Basic_Setting(TYPE _option);
	CMonsterBullet::TYPE	m_eAttackType;
	_vec3					vDir;
	_float					m_fMovingSpeed;
};

//플레이어 = -20 : 주먹
// 91 84 76 69 61 