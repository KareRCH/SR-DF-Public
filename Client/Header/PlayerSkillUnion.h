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

class CAceMonster;

class CPlayerSkillUnion : public CAttackUnion
{
	DERIVED_CLASS(CAttackUnion, CPlayerSkillUnion)

protected:
	explicit CPlayerSkillUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerSkillUnion(const CPlayerSkillUnion& rhs);
	virtual ~CPlayerSkillUnion();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

	// 성희 추가 : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(FLOAT _Width, FLOAT _Height, FLOAT Depth) override;
	virtual void		MeshSphereColider(FLOAT Radius, UINT Slices, UINT Stacks) override;

protected:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;

protected:
	struct _PLAYERSKILL	// 플레이어 스킬 구조체
	{
		_float fCurTime = 0.f;
		_float fMaxTime = 0.f;
		_float fDeleteTime = 0.f;	// 공격 삭제 시간
		_float fFrame = 0.f;		// 프레임 시작
		_float fFrameEnd = 0.f;		// 프레임 끝
		_float fFrameSpeed = 0.f;	// 프레임 속도
		_float fMoveSpeed = 0.f;	// 투사체 속도 
		_float fDamage = 0.f;		// 데미지
		_float fSize = 0.f;			// 크기

		PLAYER_ATTACK_STATE			ePlayer_AttackState; // 공격 유형

		_vec3 vPos = { 0.f, 0.f, 0.f }; // 위치
		_vec3 vDir = { 0.f, 0.f, 0.f }; // 방향
	};

	enum PLAYER_SKILLHIT
	{
		PSKILLHIT_NOHIT,
		PSKILLHIT_ONE,
		PSKILLHIT_TWO,
		PSKILLHIT_THREE,
		PSKILLHIT_END_SIZE
	};

public:
	void						Height_On_Terrain(_float Height);
	void						Billboard();

public: //플레이어 관련 셋팅 
	void						Change_PlayerHp(_float pAttack);
	HRESULT						Update_PlayerPos();
	void						Recoil(const _float& fTimeDelta, _float fSpeed);

	GAUGE<_float>				PlayerHp; //플레이어 HP 저장용 변수 
	GAUGE<_float>				MonsterHp; //몬스터 HP 저장용 변수 

public: // 몬스터 셋팅
	void						Monster_Select(CGameObject* _GameObject);
	//void						Monster_Select(CAceMonster* _AceMonster);
	_vec3						Target_Pos(CGameObject* _Target);
	void						Change_MonsterHp(_float pAttack, CGameObject* _AttackTarget, PLAYER_ATTACK_STATE _AttackState);

	GETSET_EX2(CAceMonster*, pMonster, Monster, GET, SET)
	GETSET_EX2(CGameObject*, m_pGameObject, GameObject, GET, SET)

protected:
	_PLAYERSKILL				m_tSkill;
	MonsterPhase				m_CurrPahse;
	ATTACK_TARGET				m_eAttack_Target;
	_tchar						debugString[100];
	FRAME						m_tFrame;
	PLAYER_ATTACK_STATE			m_ePlayer_AttackState;
	_vec3 m_vDir = { 0.f, 0.f, 0.f };

	PLAYER_SKILLHIT				m_ePlayer_SkillHit = PSKILLHIT_NOHIT;

	CAceMonster* pMonster = nullptr;
	CGameObject* m_pGameObject = nullptr;
};