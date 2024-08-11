#pragma once
#include "MonsterAttackUnion.h"

class CMonsterPunch : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CMonsterPunch)

	PUBLIC enum class TYPE { NORMAL, HEAVY, TYPE_END };

private:
	explicit CMonsterPunch(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonsterPunch(const CMonsterPunch& rhs);
	virtual ~CMonsterPunch();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CMonsterPunch* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
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

	// �浹 
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);

private:
	TYPE			m_eAttackType;
	_float			m_fHeavyAttack;
};

//�÷��̾� = -20 : �ָ�
// 91 84 76 69 61 