#pragma once
#include "AceMonster.h"
#include "BossLight.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"

#include "Serialize_BaseClass.h"

//공격 헤더 
#include "MonsterPunch.h"

//스킬 및 이펙트 헤더 
#include "Effect_Awareness.h"
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"
#include "RedThunder.h"
#include "MonsterBullet.h"

typedef struct BossDistance
{
	_float TotalDist = 18.f;
	
	// Intro Phase
	_float INTRO_WalkDist			= 18.f; // 15초과			= walk
	_float INTRO_ShootDist			= 15.f; // 2 초과 15 이하    = Shoot
	_float INTRO_CloseAtkDist		= 2.f;	// 2 이하			= CloseAtk

	// Phase1 
	_float Ph1_RunDist				= 13.f; // 13 초과			= run
	_float Ph1_ShootDist			= 5.f;	// 5 초과 ~ 13이하	= shoot
	_float Ph1_RollDist				= 5.f;	// 2초과 5이하		= roll + shoot
	_float Ph1_CloseAtkDist			= 2.f;	// 2 이하			= CloseAtk

	// Phase2
	_float Ph2_RunDist				= 15.f;	// 15 초과  			= run 
	_float Ph2_SideReadyDist		= 12.f;	// 12 이상 15 미만	= SideReady -> Shoot
	_float Ph2_RollDist				= 8.f;	//  8 이상 12 미만	= Roll + Shoot
	_float Ph2_ShootDist			= 8.f;	//  2 이상 8미만		= Shoot
	_float Ph2_CloseAtkDist			= 2.f;	//  2미만			= PreAtk -> CloseAtk
	
}DISTANCE;


class CAceBoss : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CAceBoss)

private:
	explicit CAceBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceBoss(const CAceBoss& rhs);
	virtual ~CAceBoss();

public:
	static CAceBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);
	static CAceBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

private:
	virtual void		Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT				Add_Component();
	void				Change_Phase();
	
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(_float, m_tStat.fAwareness, Awareness, GET, SET)
	
	// 충돌 
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;
	
	//블랙보드 - Add BlackBoard에서 추가한 BoardName과 업로드, 다운로드 이름이 모두 같아야한다. 
private:
	void	Update_InternalData(); // 블랙보드로 데이터를 올린다. 
	void	Update_BlackBoard(); // 블랙보드로부터 데이터를 받아오는용도 

private:
	// 변수 
	MonsterPhase	m_ePhase;
	DISTANCE		m_tDistance;

	_vec3			vDir; //돌려쓰는 벡터용도 
	_float			m_fTriggerHP;
	_bool			m_bPhaseStart = false;
	_float			m_fSideAge = 0.f;
	_float			m_fSideTime = 1.5f;
	_float			m_fAwareness;

	_float			m_fTalk;
	_float			m_fTalkLife;
	_float			m_fTalkReapeat;

	//공격 컨트롤 
	_bool			m_AttackOnce = false;
	_bool			m_bBuffActive = false;
	_bool			m_bShitTrigger = false;
	_bool			m_bCollisionOn = false;
	_bool			m_bMakeGun = false;

	// 조명관련 - 블랙보드 연동 
	_bool m_bLightOn = FALSE;
	void LightControl(const _float& fTimeDelta);

#pragma region 상태머신 enum셋팅

public:
	// 목표 상태머신(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, REST, CHASE,					// 경계	  (SUS = BACKIDLE)
		RELOADING, RUN, WALK,							// 추격
		
		PRE_ATTACK, SIDE_READY, ROLL, SHOUT,			// 전조 
		CLOSEATTACK,  SHOOTING, HEAVYSHOOT,				// 공격
		PHASE1_INSTALL, PHASE2_INSTALL, PHASE2_BUFF,	//스킬패턴
		FALLING_STONE, ENERGY_BALL,						//스킬패턴
		RED_THUNDER, ROUND_FIRE,						//스킬패턴
		
		HIT, DAZED, FACEPUNCH, FALLING, CROTCHHIT,		// 피격
		DEATH											// 죽음
		// 보스는 보스맵 전체가 시야범위이므로 벗어나지않음. 
	};

	// 행동 상태머신
	enum class STATE_ACT {
		IDLE, 
		APPROACH,			// RUN + WALK 
		MOVING,	FALLING,		// ROLL
		ATTACK,				// CLOSEATTACK
		SHOOT,				//
		INSTALL, BUFF,		// 관문별 1회성 스킬 
		SKILL_STONE, SKILL_ENERGYBALL,
		SKILL_THUNDER, SKILL_FIRE
	};

	// 액션키
	enum class ACTION_KEY {
		IDLE,
		RUN, WALK, ROLL, FALLING,
		CLOSEATK,
		ATTACK,  // ??버그임?? 
		SHOOT,
		SKILL_LASER, SKILL_STONE, SKILL_ENERGYBALL,
		SKILL_BUFF, SKILL_THUNDER, SKILL_FIRE //스킬
	};

private:
	STATE_SET<STATE_OBJ, void(CAceBoss*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CAceBoss*, float)> m_tState_Act;				// 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//가상 조작키 

#pragma endregion

#pragma region AI 

	// 경계 
	void AI_Idle(float fDeltaTime); // SOUTH 바라보는중 
	void AI_Suspicious(float fDeltaTime); // 오 ? 의심중
	void AI_Rest(float fDeltaTime); // 제자리에서 들썩거림
	void AI_Chase(float fDeltaTime); // 거리 체크해서 각종 상태 보내버리는 일종의 허브역할 

	// 추격 
	void AI_Run(float fDeltaTime); // 달리기 
	void AI_Walk(float fDeltaTime); // 걷기
	void AI_Reloading(float fDeltaTime); // 총알 재장전

	//전조
	void AI_PreAttack(float fDeltaTime); // 근접공격 전조  
	void AI_SideReady(float fDeltaTime); // 벽에 등지고 총잡은듯 포즈 
	void AI_Roll(float fDeltaTime); // 옆으로 구르기 

	// 공격
	void AI_CloseAttack(float fDeltaTime); // 근접공격 - 개머리판으로 후리기  
	void AI_Shoot(float fDeltaTime); // 총으로 쏘는공격 
	void AI_HeavyShoot(float fDeltaTime); // 총으로 쏘는공격 
	
	void AI_InstallPh1(float fDeltaTime); // 1관문 설치기
	void AI_InstallPh2(float fDeltaTime); // 2관문 설치기
	void AI_SkillBuff(float fDeltaTime); // 2관문 기본버프 장착 - Shout 
	
	void AI_SkillStone(float fDeltaTime); // 1관문 스킬 - stone
	void AI_SkillEnergyBall(float fDeltaTime); // 1관문 스킬 - ball 
	void AI_SkillThunder(float fDeltaTime); // 2관문 스킬
	void AI_SkillFire(float fDeltaTime); // 2관문 스킬
	
	// 피격
	void AI_Hit(float fDeltaTime); // 맞은 히트판정 
	void AI_Dazed(float fDeltaTime); // hp 일정이상 닳은 상태 
	void AI_Falling(float fDeltaTime); // 발차기 맞았을경우 
	void AI_FacePunch(float fDeltaTime); // 얼굴에 맞았을경우 
	void AI_CrotchHit(float fDeltaTime); // 하단공격  

	// 죽음 
	void AI_Death(float fDeltaTime); //ok // hp 0인상태 
	//Engine::Play_Sound(L"Enemy", L"_Death.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// ROLL
	void Attack(float fDeltaTime);			// CloseATK
	void Shoot(float fDeltaTime);			// Shoot
	void Falling(float fDeltaTime);		    // Falling	

	void LaserInstall(float fDeltaTime);	// 레이저 설치
	void BuffActive(float fDeltaTime);		// 버프 추가  

	void SkillStone(float fDeltaTime);		// 스킬  
	void SkillEnergyBall(float fDeltaTime);	// 스킬  
	void SkillThunder(float fDeltaTime);	// 스킬  
	void SkillFire(float fDeltaTime);		// 스킬  


#pragma endregion

};


/* 스킬 공격력 정리 
Falling Stone = 3
Fire = 4
EnergyBall = 4
Thunder = 6

일반 Atk = 3
Heavy Atk = 5
*/


/* SPEED 정리

디버그 라인

OutputDebugString(L"▶ : 충돌 관련 디버그
OutputDebugString(L"▷ : 상태머신 관련 디버그
OutputDebugString(L"★★★★★  중요 디버그라인 \n");

swprintf_s(debugString, L"Boss - 변수 확인 m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

10단위 
phase1 = energyball
phase2 = thunder 


*/