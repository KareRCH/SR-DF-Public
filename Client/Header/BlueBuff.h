#pragma once
#include "MonsterAttackUnion.h"

class CBlueBuff : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CBlueBuff)

private:
	explicit CBlueBuff(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBlueBuff(const CBlueBuff& rhs);
	virtual ~CBlueBuff();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CBlueBuff* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		_float _x, _float _y, _float _z, MonsterPhase _CurrPhase, CGameObject* pOwner, ETEAM_ID _eTeamid);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	virtual void OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionExited(CGameObject* pDst) override;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	
private:
	_bool m_bBossDead = FALSE;
};
