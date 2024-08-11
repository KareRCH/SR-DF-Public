#pragma once

#include "AceMonster.h"
#include "Effect_Awareness.h"
#include "ThrowPipe.h"
#include "MonsterPunch.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CGray : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CGray)

private:
	explicit CGray(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGray(const CGray& rhs);
	virtual ~CGray();

public:
	static CGray* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);
	static CGray* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial);

private:
	virtual void		Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual HRESULT Ready_GameObject(const FSerialize_GameObject tObjectSerial);
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT				Add_Component();
	void				RenderSplitImages();

	// Get, Set 함수 
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

	//블랙보드 - Add BlackBoard에서 추가한 BoardName과 업로드, 다운로드 이름이 모두 같아야한다. 
private:
	void	Update_InternalData(); // 블랙보드로 데이터를 올린다. 
	void	Update_BlackBoard(); // 블랙보드로부터 데이터를 받아오는용도 


#pragma region 상태머신 enum셋팅

public:
	// 목표 상태머신(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, YOUDIE, TAUNT, CHASE, REST,			//추격
		RUN, WALK, KEEPEYE, SIDEWALK, FRIGHTEN, UPRIGHTRUN,		//견제
		THROW, ATTACK,   HEAVYATTACK,							//공격
		RECONNAISSANCE, GOHOME,									//복귀
		BLOCK,													//막기
		CROTCHHIT, FACEPUNCH, FALLING, HIT,						//피격
		DAZED, DEATH, CHOPPED, HEADSHOT, HEADLESS				//사망
		//미구현 리스트 
	};

	// 행동 상태머신
	enum class STATE_ACT {
		IDLE, APPROACH, SUDDENATTACK, SIDEMOVING, ATTACK,
		FALLING,
		GOHOME
	};

	//가상키 
	enum class ACTION_KEY {
		IDLE, RUN, WALK, KEEPEYE, SIDEWALK,
		UPRIGHT, FRIGHTEN, BASIC_ATTACK, HEAVY_ATTACK,
		BACK,
		GOHOME
	};

#pragma endregion 

private:
	// 사거리 
	_float		m_fRunDistance = 8.f;				// 사거리 ~ Run 사이 =  run
	_float		m_fWalkDistance = 7.5f;				// run~walk 사이 = walk
	_float		m_fEyesOnYouDistance = 6.f;			// Eyes ~ Walk = 옆으로 무빙 
	_float		m_fCloseToYouDistance = 2.f;		// Close ~ Eyes = 

	// 위치 조절 - 돌려쓰는용도 
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  
	
	//스위치 on/off 
	_bool		m_bArrive = FALSE; // 집에 도착 여부 
	_bool		m_AttackOnce = FALSE;
	_bool		m_bSecondFall = FALSE;

	// 외부타격으로 인한 죽음
	enum class RECENT_COL { PLAYER, PLAYERATK, BOSSATK, RECEND_END };

	void			MonsterDead();
	RECENT_COL		m_eRecentCol; // 플레이어1 플레이어공격체2 보스스킬3 

public:
	using state_set_object = STATE_SET<STATE_OBJ, void(CGray*, float)>;
	GETSET_EX1(state_set_object, m_tState_Obj, StateObj, GET_REF)

private:
	//상태머신 등록 
	STATE_SET<STATE_OBJ, void(CGray*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CGray*, float)> m_tState_Act;				// 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey;

#pragma region AI 

	void AI_Idle(float fDeltaTime); // 처음 서있는 용도 
	void AI_Suspicious(float fDeltaTime); // 견제값 추가용 
	void AI_YouDie(float fDeltaTime); //도발
	void AI_Taunt(float fDeltaTime); //도발
	void AI_Chase(float fDeltaTime); //  거리비교 움직임 시작
	void AI_Rest(float fDeltaTime); // idle ready상태 - 중간중간 넣기용 

	void AI_Run(float fDeltaTime); //빠르게 달려오기
	void AI_Walk(float fDeltaTime); //일반적인 걷기 
	void AI_KeepEye(float fDeltaTime); //플레이어 주시한채로 백스탭밟기 느리게 
	void AI_SideWalk(float fDeltaTime); //플레이어 주시한채로 옆으로 걸음 - 조금 빠르게 walk 
	void AI_Frighten(float fDeltaTime); //점프해서 때림 - 점프하는것 여기에  공격이 바로 연타되어야할듯
	void AI_UpRightRun(float fDeltaTime); //파이프 들고 공격하러 달려오는상태

	void AI_Throw(float fDeltaTime); //파이프 던짐 
	void AI_Attack(float fDeltaTime); // 파이프 들고 대각선으로 걍 떄리는거
	void AI_HeavyAttack(float fDeltaTime); //강공격 - 옆으로 스매시

	void AI_Reconnaissance(float fDeltaTime); // 플레이어 놓쳐서 주변 정찰하는중 
	void AI_GoHome(float fDeltaTime); // 파이프 들고 대각선으로 걍 떄리는거
	
	void AI_Block(float fDeltaTime); //플레이어가 공격하면 막기함

	void AI_CrotchHit( float fDeltaTime); // 앉기 + 피격
	void AI_FacePunch(float fDeltaTime); //얼굴 피격
	void AI_Falling(float fDeltaTime); //떨어지고 일어나는것
	void AI_Hit(float fDeltaTime); //일정피 이하가 되면 나오는상태
	
	void AI_Dazed(float fDeltaTime); //일정피 이하가 되면 나오는상태
	void AI_Death(float fDeltaTime); //일반공격으로 죽는상태 
	void AI_Chopped(float fDeltaTime); //뒤돌고 있을때 손날치기 -> sleep 
	void AI_HeadShot(float fDeltaTime); // 주먹이나 기타 무기로 헤드샷
	void AI_Headless(float fDeltaTime); //총으로 헤드어택
	
	/*
	
	Engine::Play_Sound(L"Enemy", L"Gray_FacePunchA.wav", SOUND_ENEMY_MONSTER, m_tSound.m_fSoundVolume);
	 
	 SOUND_ENEMY_MONSTER, SOUND_ENEMY_MONSTER_EFFECT, 
	 */
#pragma endregion

#pragma region AI 행동 

	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime); // RUN + WALK 
	void SuddenAttack(float fDeltaTime);  // UPRIGHT + FRIGHTEN
	void SideMoving(float fDeltaTime); // EKKPEYE + SIDEWALK
	void Attack(float fDeltaTime); // BASIC ATTACK + HEAVY ATTACK
	void GoHome(float fDeltaTime); // GOHOME 
	void Falling(float fDeltaTime); // GOHOME 

#pragma endregion
};

/* SPEED 정리

Speed 1 
Speed 2 
Speed 3 = GoHome,KEEP EYE
Speed 4 = walk, SideWalk
Speed 5 = UpRight
Speed 6 = run, frighten
Speed 7

swprintf_s(debugString, L"Gray - 변수 확인 m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/