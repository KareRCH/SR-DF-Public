#pragma once

#include "AceMonster.h"
#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"
#include "Effect_Awareness.h"
#include "MonsterPunch.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CBrown : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CBrown)

private:
	explicit CBrown(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBrown(const CBrown& rhs);
	virtual ~CBrown();

public:
	static CBrown* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);
	static CBrown* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial);

private:
	virtual void	Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Ready_GameObject(const FSerialize_GameObject tObjectSerial);
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT				Add_Component();

	// Get, Set 함수 만들기 
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

	//충돌 디버그용
public:
	LPD3DXMESH		m_pMesh = nullptr;

	//블랙보드
private:
	void	Update_InternalData(); // 블랙보드로 데이터를 올린다. 
	void	Update_BlackBoard(); // 블랙보드로부터 데이터를 받아오는용도 

protected:  
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster; // 블랙보드 몬스터 
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;	// 블랙보드 플레이어

private:
	// 거리
	_float		m_fRunDistance = 6.f;				// 사거리 ~ Run 사이 =  run
	_float		m_fWalkDistance = 5.5f;				// run~walk 사이 = walk
	_float		m_fInchDistance = 1.5f;  

	// 위치 조절 
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  
	
	//스위치 on/off 
	_bool		m_bArrive = FALSE;
	_bool		m_AttackOnce = FALSE;
	_bool		m_bCollisionEnter = TRUE;
	_bool		m_bSecondFall = FALSE;
	// 외부타격으로 인한 죽음
	enum class RECENT_COL {PLAYER, PLAYERATK, BOSSATK, RECEND_END};
	
	void MonsterDead();
	RECENT_COL		m_eRecentCol; // 플레이어1 플레이어공격체2 보스스킬3 
	

#pragma region 상태머신 enum셋팅
	
public: 
	// 목표 상태머신(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, TAUNT, REST, CHASE, 		// 경계	 
		RUN, WALK, STRAFING, INCHFORWARD, 			// 추격
		NORMALATTACK, HEAVYATTACK, 					// 공격
		HIT, FACEPUNCH, CROTCHHIT,					// 피격
		DAZED, FALLING, CHOPPED, DEATH, HEADLESS,	// 죽음
		RECONNAISSANCE, GOHOME,						// 복귀

		//미구현 리스트 
		JUMP
	};

	// 행동 상태머신
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, GOHOME, FALLING
	};

	// 행동키
	enum class ACTION_KEY { 
		IDLE, 
		RUN, WALK, INCHFORWARD, STRAFING, 
		JUMP, BEPUSHED,
		NORMALATTACK, HEAVY_ATTACK,
		GOHOME 
	};
	using state_set_object = STATE_SET<STATE_OBJ, void(CBrown*, float)>;
	GETSET_EX1(state_set_object, m_tState_Obj, StateObj, GET_REF)

private:
	STATE_SET<STATE_OBJ, void(CBrown*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act;				// 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//가상 조작키 

#pragma endregion

#pragma region AI 

	// 경계 
	void AI_Idle(float fDeltaTime); // SOUTH 바라보는중 
	void AI_Suspicious(float fDeltaTime); // 오 ? 의심중
	void AI_Taunt(float fDeltaTime); // 다가와봐 하고 도발상태 
	void AI_Chase(float fDeltaTime); // 거리 체크해서 각종 상태 보내버리는 일종의 허브역할 
	void AI_Rest(float fDeltaTime); // 제자리에서 들썩거림

	// 추격 
	void AI_Run(float fDeltaTime); // 달리기 
	void AI_Walk(float fDeltaTime); // 걷기
	void AI_Strafing(float fDeltaTime); // 좌우로 흔들면서 옆, 뒤 둘중 하나로 이동 
	void AI_InchForward(float fDeltaTime); // 앞으로 훅 다가오는 복싱 자세 
	
	// 공격
	void AI_HeavyAttack(float fDeltaTime); // 강공격 
	void AI_NormalATTACK(float fDeltaTime); // 일반 공격
	
	// 피격
	void AI_Hit(float fDeltaTime); // 맞은 히트판정 
	void AI_FacePunch(float fDeltaTime); // 얼굴에 맞았을경우 
	void AI_Falling(float fDeltaTime); // 발차기 맞았을경우 
	void AI_CrotchHit(float fDeltaTime); // 하단 맞았을경우  

	// 죽음 
	void AI_Dazed(float fDeltaTime); // hp 일정이상 닳은 상태 
	void AI_Death(float fDeltaTime); //ok // hp 0인상태 
	void AI_Chopped(float fDeltaTime); // 뒤에서 플레이어가 기습공격했을경우 Sleep으로 들어감 
	void AI_Headless(float fDeltaTime); //ok // 총류로 머리를 맞았을경우 

	// 복귀
	void AI_Reconnaissance(float fDeltaTime); // 플레이어 놓쳐서 주변 정찰하는중 
	void AI_GoHome(float fDeltaTime);		 // 정찰마치고 원위치 복귀중 
	/*

	 Engine::Play_Sound(L"Enemy", L"Gray_FacePunchA.wav", SOUND_ENEMY_MONSTER, m_tSound.m_fSoundVolume);
	 
	  SOUND_ENEMY_MONSTER, SOUND_ENEMY_MONSTER_EFFECT,
 */
#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_NORMALATTACK + AI_HeavyAttack
	void Falling(float fDeltaTime);			// Gohome
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion

};

/* SPEED 정리

Speed 1 = 
Speed 2 = 
Speed 3 = 
Speed 3.5 = WALK 
Speed 4 = 
Speed 5 = RUN
Speed 6 = GoHome
Speed 7 = Strafing
Speed 8 = Inch 
Speed 9 =

*/

/*
디버그 라인 

OutputDebugString(L"▶ : 충돌 관련 디버그 
OutputDebugString(L"▷ : 상태머신 관련 디버그 

swprintf_s(debugString, L"Brown - 변수 확인 m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/