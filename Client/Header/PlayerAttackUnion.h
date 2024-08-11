#pragma once
#include "AttackUnion.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CPlayerAttackUnion : public CAttackUnion
{
	DERIVED_CLASS(CAttackUnion, CPlayerAttackUnion)

protected:
	explicit CPlayerAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerAttackUnion(const CPlayerAttackUnion& rhs);
	virtual ~CPlayerAttackUnion();

private:
	struct _PLAYERATTACK	// 플레이어 공격 구조체
	{
		_float fCurTime = 0.f;
		_float fFrame = 0.f;		// 프레임 시작
		_float fFrameEnd = 0.f;		// 프레임 끝
		_float fFrameSpeed = 0.f;	// 프레임 속도
		_float fMoveSpeed = 0.f;	// 투사체 속도 
		_float fDeleteTime = 0.f;	// 공격 삭제 시간
		_float fDamage = 0.f;		// 데미지
		_float fSize = 0.f;			// 크기

		PLAYER_ATTACK_STATE			ePlayer_AttackState; // 공격 유형

		_vec3 vDir = { 0.f, 0.f, 0.f }; // 방향
	};

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

	// 성희 추가 : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(_float _Width, _float _Height, _float Depth) override;
	virtual void		MeshSphereColider(_float Radius, _uint Slices, _uint Stacks) override;

protected:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;



public:
	_PLAYERATTACK Get_AttackState() 
	{
		return m_tAttack;
	} 
	void Set_PlayerAttackState(_vec3 vDir, PLAYER_ATTACK_STATE _AttackState,
						 _float fMoveSpeed, _float fDeleteTime, _float fDamage, _float fSize)
	{
		//m_tAttack.vDir = vDir;
		m_tAttack.vDir.x = vDir.x;
		m_tAttack.vDir.y = vDir.y;
		m_tAttack.vDir.z = vDir.z;

		m_tAttack.ePlayer_AttackState = _AttackState;
		m_tAttack.fMoveSpeed = fMoveSpeed;
		m_tAttack.fDeleteTime = fDeleteTime;
		m_tAttack.fDamage = fDamage;
		m_tAttack.fSize = fSize;
	}

public:
	void						Height_On_Terrain(_float Height);
	void						Billboard();

public: //플레이어 관련 셋팅 
	void						Change_PlayerHp(_float pAttack);
	_bool						Change_MonsterHp(_float pAttack, CGameObject* _AttackTarget,
		PLAYER_ATTACK_STATE _AttackState, STATE_RIGHTHAND _attacktype);
	HRESULT						Update_PlayerPos();
	void						Recoil(const _float& fTimeDelta, _float fSpeed);
	GAUGE<_float>				PlayerHp; //플레이어 HP 저장용 변수 
	GAUGE<_float>				MonsterHp; //몬스터 HP 저장용 변수 

protected:
	_PLAYERATTACK				m_tAttack;
	
};