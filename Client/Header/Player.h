#pragma once

#include "AceUnit.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoard_Camera.h"
#include "BlackBoardPtr.h"
//임시용
#include "AceFood.h" 
#include "PlayerBullet.h"
#include "CloseAttack.h"
#include "MeshColComp.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCameraComponent;
class CCalculatorComponent;
class CSphereColComp;
class CMeshColComp;

END

class CDynamicCamera;
class CPlayerLighter;
class CBrown;
class CGray;

/// <summary>
/// 플레이어 생성용 구조체
/// vPos, vRot, vScale, fPriority[3]
/// </summary>
struct FPlayer_Create
{
	_vec3 vPos = { 0.f, 0.f, 0.f };
	_vec3 vRot = { 0.f, 0.f, 0.f };
	_vec3 vScale = { 1.f, 1.f, 1.f };
	_float fPriority[static_cast<_uint>(EPRIORITY_TYPE::SIZE)] = {0.f, 0.f, 0.f};
};

typedef struct tag_PlayerSound_Info
{
	_float		m_fTalkAge;				// 말하는 시간 - 나이
	_float		m_fTalkLife;			// 말하는시간 - 수명 
	_float		m_fTalkReapeat;			// 말하는시간  - 반복횟수 카운트 

	_float		m_fSoundVolume;			//	사운드 재생하는 볼륨 
	_float		m_fSoundEffectVolume;	//	이펙트 재생볼륨
	
	_int		m_iHpSection;			// 구간나누기 위한 섹션 정의 - 20으로 나눈구간임 

	_bool		m_bSoundOnce;			// 한번만 재생되어야할때 
	_bool		m_bSoundCheck;

}PlayerSound;

class CPlayer : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CPlayer)
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

public:
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;

	// ===============================랜 더================================
	virtual void		Render_GameObject() override;
	// ====================================================================

	// =============================상태 추가==============================
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	// ====================================================================
private:
	// ============================손 상태 체크============================
	void				Hand_Check();
	// ====================================================================

	// =============================양손 주먹==============================
	void				Two_Hand();
	// ====================================================================

	// ===========================양손 오브젝트============================
	void				Two_Object();
	// ====================================================================

	// ===========================한손 오브젝트============================
	void				Right_Object();
	// ====================================================================

	// ============================키보드 입력=============================
	bool				Keyboard_Input(const _float& fTimeDelta);	// 키보드 입력
	// ====================================================================

	// ============================== 프레임 ==============================
	void				FrameManage(const _float& fTimeDelta);	// 프레임 관리
	// ====================================================================

	// =============================== 차징 ===============================
	void				Charge(const _float& fTimeDelta);	// 차징
	// ====================================================================

	// =============================== 피킹 =============================== // 소영 추가 
	//_vec3				Picking_On_Object();	// 오브젝트 마우스 피킹 
	_bool				Picking_On_Object();
	// ====================================================================


public: // 블랙보드
	void		Update_BlackBoard();
	void		Update_InternalData();
	_vec3 Test = { 0.f, 10.f, 0.f };
//-------------------------------------------------------------------------------------------------

protected: // 충돌 onoff
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void	OnCollisionExited(CGameObject* pDst) override;

//-------------------------------------------------------------------------------------------------

public: // Get_Set
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pLeftHandComp, m_pLeftHandComp, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pRightHandComp, m_pRightHandComp, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET) // 충돌 필수 
	//GETSET_EX2(CPlayerLighter*, m_PlayerLighter, PlayerLighter, GET, SET)	// 라이터 조명

	// 플레이어 오른손 상태값
	//enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	// 플레이어 상태값
	//enum class STATE_PLAYER { NONE, IDLE, SITDOWN, JUMP, DIE, PLAYER_STATE_SIZE_END };
	// 플레이어 행동 상태값
	enum class STATE_PLAYER_ACTION { IDLE, RUN, GUARD, ATTACK, CHARGING, THROW_AWAY, PLAYER_ACTION_SIZE_END };

	// 소영 추가 ---------------- 
	GETSET_EX2(STATE_RIGHTHAND, m_eRIGHTState, PlayerRightHand, GET, SET)   // 오른손 상태값 받아오는용도 
	GETSET_EX2(GAUGE<_float>, m_gHp, PlayerHP, GET, SET)   // 플레이어 hp용도 
	GETSET_EX2(_bool, m_bAttack, PlayerAttackBool, GET, SET)   // 플레이어 공격  

	STATE_RIGHTHAND   m_eRIGHTState;   // 오른손상태
	
	_float m_fAge = 0.f;
	_float m_fLifeTime = 0.2f;
	_bool m_bGunLight; // 불켜는 bool값 
	_bool m_bHitState = false;
	_bool m_bMagicBottle = false;

	void CrossHairState();
	// ------------------------- 

		_bool* Get_DBugFrame() {
		return &bDbugFrame;
	} void Set_DBugFrame(_bool* value) {
		bDbugFrame = value;
	}	// 스위치

		
private: // 컴포넌트
	// 기능
	CRcBufferComp* m_pBufferComp = nullptr;					// Rc버퍼
	CTransformComponent* m_pTransformComp = nullptr;		// 이동행렬 버퍼
	CCalculatorComponent* m_pCalculatorComp = nullptr;		// 지형 타기 컴포넌트
	CColliderComponent* m_pColliderComp = nullptr;				// 구 충돌 콜라이더

	// 플레이어 손
	CTextureComponent* m_pLeftHandComp = nullptr;			// 왼손 텍스처(담을곳)
	CTextureComponent* m_pRightHandComp = nullptr;			// 오른손 텍스처(담을곳)


//-------------------------------------------------------------------------------------------------

private:
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;
	FBlackBoardPtr<CBlackBoard_Camera>		m_wpBlackBoard_Camera;

//-------------------------------------------------------------------------------------------------

public:// 플레이어 상태 값
	// TEST
	enum class OBJECT_TYPE { NONE, TWO_HAND, TWO_OBJECT, RIGHT_OBJECT }; // 테스트(오브젝트)
	enum class OBJECT_NAME { NONE, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN };
	
	CAceFood::FOOD_NAME m_eFoodName;

	// TEST
	enum DASHDIR { LEFT, RIGHT, DOWN };	// 대쉬 방향 

private:
	enum class EACTION_KEY : _uint { RIGHT, LEFT, UP, DOWN, RUN, ATTACK, JUMP, SITDOWN, SIZE };
	ACTION_SET<EACTION_KEY>			m_tActionKey;

public:
	//enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, CHARGING, KICK, THROW_AWAY, DIE, JUMP, PLAYER_END };
	//enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	enum class STATE_LEFTHAND { NONE, HAND, OPEN_HAND, RUN_HAND, RIGHTER, RUN_RIHGTER };

private: // 플레이어의 상태 머신 (상태x, 뛰기, 앉기, 점프, 죽음)
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;
	STATE_PLAYER m_ePlayerTest;
	void Idle(float fTimeDelta);
	void Down(float fTimeDelta);
	void Kick(float fTimeDelta);
	void Die(float fTimeDelta);

private: // 플레이어의 행동 머신 (행동x, 이동, 공격, 차징, 버리기)
	STATE_SET<STATE_PLAYER_ACTION, void(CPlayer*, float)> m_tPlayer_Action;

	void Action_Idle(float fTimeDelta);
	void Action_Move(float fTimeDelta);
	void Action_Sitdown(float fTimeDelta);
	void Action_Run(float fTimeDelta);
	void Action_Charging(float fTimeDelta);
	void Action_ThrowAway(float fTimeDelta);

public:
	STATE_PLAYER Get_PlayerState() { return m_tPlayer_State.Get_State(); }

private: // 플레이어의 왼손 상태 머신
	STATE_SET<STATE_LEFTHAND, void(CPlayer*, float)> m_tLeftHand_State;
	STATE_LEFTHAND m_eLeftState_Old;

	void	Left_None(float fTimeDelta);
	void	Left_Hand(float fTimeDelta);
	void	Left_RunRighter(float fTimeDelta);
	void	Left_OpenHand(float fTimeDelta);
	void	Left_RunHand(float fTimeDelta);
	void	Left_Righter(float fTimeDelta);

private: // 플레이어의 오른손 상태 머신
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightHand_State;
	//STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightState_Old;
	STATE_RIGHTHAND m_eRightState_Old;

	void	Right_None(float fTimeDelta);
	void	Right_Hand(float fTimeDelta);
	void	Right_RunHand(float fTimeDelta);
	void	Right_Gun(float fTimeDelta);
	void	Right_Thompson(float fTimeDelta);
	void	Right_Steelpipe(float fTimeDelta);
	void	Right_BeerBotle(float fTimeDelta);
	void	Right_FryingPan(float fTimeDelta);
	void	Right_Kick(float fTimeDelta);

private: // 함수
	HRESULT				Add_Component();									// 컴포넌트 추가
	//bool				Keyboard_Input(const _float& fTimeDelta);			// 키보드 입력
	bool				Attack_Input(const _float& fTimeDelta);				// 공격 입력(마우스)
	void				Mouse_Move();										// 마우스 움직임
	void				Height_On_Terrain();								// 지형타기
	void				Dash(const _float& fTimeDelta);
	//void				Hand_Check();										// 플레이어 손 상태 체크
	void				LeftLoadAnimationFromFile(const char* fileName);	// 애니메이션 불러오기
	void				RightLoadAnimationFromFile(const char* fileName);	// 애니메이션 불러오기
	void				LeftInterpolation();
	void				RightInterpolation();
	void				LineEvent();
	void				RayAttack(CGameObject* _pDst, _float _fAttack, PLAYER_ATTACK_STATE _eAttackState);
	CGameObject*		RayCast();

private:
	void State_Update(float fTimeDelta);	// 상태 업데이트(상태 체크)

private: // 스위치
	_bool		bSpinOn = false;			// 총 회전 On/Off
	_bool		bRighter = false;			// 라이터 On/Off
	_bool		bRightHandOn = true;		// 오른손 출력 On/Off
	_bool		bRightGetAnimation = false;	// 애니메이션 불러오기 On/Off
	_bool		bLeftGetAnimation = false;	// 애니메이션 불러오기 On/Off

	// 대쉬
	_bool		bDashOn = false;		// 플레이어 대쉬 여부
	_bool		bDashChange = false;	// 대쉬 증감 On/Off

	// 주먹 공격
	_bool		bLeftPunch = false;		// 왼주먹 On/Off
	_bool		bRightPunch = true;		// 오른주먹 On/Off

	_bool		bShieldOn = true;		// 쉴드 On/Off
	_bool		bChargeAttack = false;	// 일반 공격에서 차징 공격으로 변경
	_bool		bBackRighter = false;	// 라이터 되돌리기
	_bool		bDbugFrame = false;		// 디버그 프레임
	_bool		m_bAttack = false;		// 공격 On/Off
	_bool		m_bAttackRotOn = false;

private:
	struct _LEFTHAND	// 왼손
	{
		_bool			bLeftHandOn = true;			// 왼손 출력 On/Off
		_bool			bLeftFrameOn = false;
		_bool			bLeftAttacColOn = false;
		_bool			bPickUpState = false;

		_float			fLeftFrameSpeed = 10.f;		// 왼손 프레임 속도
		_float			fLeftFrame = 0.f;			// 왼손 프레임
	};

	struct _RIGHTHAND	// 오른손
	{
		_bool			bRightFrameOn = false;
		_bool			bPickUpState = false;
		_bool			bRightAttacColOn = false;
		_bool			bBasicAttackOn = false;
		_bool			bChargingAttackOn = false;

		_float			fRightFrameSpeed = 1.f;		// 오른손 프레임 속도
		_float			fRightFrame = 0.f;			// 오른손 프레임

		_uint			iFullChargingIndex = 0;	// 풀차지시 인덱스
		_uint			iShieldIndex = 0;			// 쉴드시 인덱스
	};

	struct _PLAYER	// 플레이어
	{
		_float		fStraightSpeed = 5.f;		// 플레이어 전진 속도
		_float		fSpeed = 5.f;				// 플레이어 속도
	};

	struct _DASH	// 대쉬
	{
		_float		fDash = 50.f;				// 플레이어 대쉬
		_float		fDownDash = 0.f;			// 플레이어 대쉬할때 높이
	};

	struct _TIME	// 시간
	{
		_float		fCurrentTime = 0.f;			// 현재 시간
		_float		fCurChangeTime = 1.f;		// 시간 속도 조절(배율)

		_float		fLeftCurrentTime = 0.f;		// 왼손 현재 시간
		_float		fLeftMaxTime = 0.f;			// 왼손 최대 시간
		_float		fLeftChangeTime = 1.f;		// 시간 속도 조절(배율)

		_float		fRightCurrentTime = 0.f;	// 오른손 현재 시간
		_float		fRightMaxTime = 0.f;		// 오른손 최대 시간
		_float		fRightChangeTime = 1.f;		// 시간 속도 조절(배율)

		_float		fMaxChangeTime = 3.f;		// 변경될 최대 시간
		_float		fChargeStartTime = 0.f;		// 차지를 시작할 시간

		_float		fAttackCreateTime = 0.f;	// 공격 생성 시간
	};

	struct _CAMERA
	{
		_bool bOne = false;
		_bool bThree = false;

		_vec3 vEye = { 0.f, 0.f, 0.f };
		_vec3 vAt = { 0.f, 0.f, 0.f };
	};

	PLAYER_ATTACK_STATE m_eAttackState; // 플레이어 공격 유형

	_PLAYER			m_tPlayer;		// 플레이어
	_LEFTHAND		m_tLeftHand;	// 플레이어 왼손
	_RIGHTHAND		m_tRightHand;	// 플레이어 오른손
	_DASH			m_tDash;		// 대쉬
	_TIME			m_tTime;		// 시간
	_CAMERA			m_tCamera;		// 카메라

	GAUGE<_float>		m_fChage;	// 차지

	// 플레이어가 바라보는 방향
	//_vec3	vPlayerLook = { 0.f, 0.f, 0.f };

	// Test
	OBJECT_TYPE m_eObjectType;	// 오브젝트 타입
	OBJECT_NAME m_eObjectName;	// 오브젝트 이름

	// 대쉬 방향
	DASHDIR m_eDashDir;

	// 라이터 조명
	_bool m_PlayerLighter;
private:
		// 애니메이션 타임 라인
		std::vector<KEYFRAME> timeline[KEYTYPE_END];

private:
	_long			dwMouseMove = 0;		// 마우스 무브
	CMeshColComp*	m_pMeshComp = nullptr;
	LPD3DXMESH*		m_MeshBox;

private:
	PlayerSound m_tPlayerSound;

private:
	virtual void		Free();

private: // 보간 변수
	// dt 값
	_float fFrameTimeDelta, fCurFrameTimeDelta;
	// 크기
	_float fSizeX_Delta, fSizeY_Delta;
	// 회전
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;
	// 이동
	_float fPosX_Delta, fPosY_Delta;

private:
	void PlayerJump(float fTimeDelta);

private:
	enum EBLACKBOARD_UPDATE
	{
		EBLACKBOARD_NONE,
		EBLACKBOARD_UPLOAD,
		EBLACKBOARD_DOWNLOAD
	};

private:
	_vec3 m_vRotPlus = { 0.f, 0.f, 0.f };
	_vec3 m_vRotMinus = { 0.f, 0.f, 0.f };

	_vec3 m_vCurLook = { 0.f, 0.f, 0.f };

	_bool m_bRotChange = false;
	_bool m_bRotStart = false;
	_bool m_bLRotStart = false;

	_bool m_bAttackCreateOn = false;

	_int m_bLAttackMove = EBLACKBOARD_NONE;
	_int m_bRAttackMove = EBLACKBOARD_NONE;

	_float fRotStart = 0.f;
	_float fLRotStart = 0.f;

	_float fAttackTime = 0.f;

	_vec3 m_vIdlePos = { 0.f, 0.f, 0.f };
	_bool m_bHandSwitch = false;
private:
	GAUGE<_float>	m_fMonsterHp;


	_bool Random_variable(_int _winning)
	{
		//운영체제 단에서 제공하는 진짜 난수 - 컴퓨터가 주변의 환경과 무작위적으로 상호작용하면서 만드는것
		// 의사난수보다 난수를 생성하는 속도가 매우 느리다. 
		random_device rd;

		// 메르센 트위스터라는 알고리즘을 사용해서 난수 생성 엔진을 초기화
		mt19937 gen(rd());

		// 0 부터 99 까지 연속확률분포 생성   (min, max)
		uniform_int_distribution<_int> iDistribution(0, 99);

		//랜덤값 생성 
		_int RandomValue = iDistribution(gen);

		//매개변수가 5일경우 5프로 확률로 true가 반환된다
		return (RandomValue <= _winning);
	}
};

/*	현재 키 설명

================ 카메라 ================
F1 : 1인칭 카메라
F2 : 3인칭 카메라
F3 : 자유시점 카메라

방향키 : 카메라 이동
========================================

=============== 플레이어 ===============

-이동-
W : 전진
S : 후진
A : 좌회전
D : 우회전

W + LShift : 달리기
A, S, D + LShift : 대쉬

-플레이어 기능-
Q : 발차기
V : 라이터 (양손 오브젝트일 경우 불가능)
R : 권총 회전(권총 상태일때만 가능)

-오브젝트 변경-
1 : 주먹
2 : 권총
3 : 톰슨 기관총
4 : 쇠파이프
5 : 맥주병
6 : 프라이팬

-공격-
MouseLB 1클릭 : 단일 공격
MouseLB 꾹 누르기 : 차징 공격 (차징 공격이 가능한 상태만)
MouseRB : 가드 (가드가 가능한 상태만)
========================================

*/

//private: // 쿼터니언
	//_long dwMouseMoveX;
	//_long dwMouseMoveY;

	//D3DXQUATERNION	m_quaternion;	// 쿼터니온 변수