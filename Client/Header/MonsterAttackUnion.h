#pragma once
#include "AttackUnion.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Define.h"

#include "AceMonster.h"
#include "UI_PlayerHurt.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CMonsterAttackUnion : public CAttackUnion
{
	DERIVED_CLASS(CAttackUnion, CMonsterAttackUnion)

protected:
	explicit CMonsterAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonsterAttackUnion(const CMonsterAttackUnion& rhs);
	virtual ~CMonsterAttackUnion();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

protected:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;

public:
	void						Height_On_Terrain(_float Height);
	void						Billboard();

public:
	void						Change_PlayerHp(_float _fAttack);
	void						Knockback_Player(const _float& fTimeDelta, _float _fSpeed);
	void						Change_MonsterHp(_float _fAttack);
	_bool						Attack_Occurrence(CGameObject* pDst, _float fAttack);
	GAUGE<_float>				m_gPlayerHp; //�÷��̾� HP ����� 
	GAUGE<_float>				m_gMonsterHp; //���� HP �����
	void						Owner_Dead(CGameObject* pOwner);

protected:
	_tchar						debugString[100];	// ����׿� string
	MonsterPhase				m_CurrPahse;		// ��ȭ���� ����� 
	FRAME						m_tFrame;			// �����ӱ���ü 
	_vec3						vPlayerPos;			// �÷��̾� ��ġ ���� 
	_float						m_fAttack;			// ��ų�� ���ݷ� 
	FCollisionSphere*			pSphereShape;
	FCollisionSphere*			pBoxShape;
	_bool						m_bOwnerDead;
};