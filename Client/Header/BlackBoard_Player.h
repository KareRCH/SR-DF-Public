#pragma once

#include "BlackBoard.h"

class CPlayer;

/// <summary>
/// 이니셜라이즈용 구조체
/// </summary>
struct FBlackBoard_PlayerInit
{
	CPlayer* pPlayer;
};


/// <summary>
/// 플레이어 전용 블랙보드
/// 플레이어에게 필요한 정보라면 여기에 업데이트 하자
/// </summary>
class CBlackBoard_Player : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Player)
	// 플레이어 오른손 상태값
	//enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	// 플레이어 상태값
	//enum class STATE_PLAYER { NONE, IDLE, SITDOWN, JUMP, DIE, PLAYER_STATE_SIZE_END };

protected:
	explicit CBlackBoard_Player();
	explicit CBlackBoard_Player(const CBlackBoard_Player& rhs);
	virtual ~CBlackBoard_Player();

public:
	static CBlackBoard_Player* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// 초기화용 구조체를 만들어 매개변수로 받게 한다.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_PlayerInit& tInit);

public:		// 플레이어에 대한 정보만 얻을 때는 
	GETSET_EX1(CPlayer*, m_pPlayer, Player, GET)

private:
	CPlayer*			m_pPlayer = nullptr;

public:		// 데이터 가공의 성격을 띌때는 해당 함수를 직접 만들것.
	GETSET_EX2(GAUGE<_float>, m_fHP, HP, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_fStamina, Stamina, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bGunLight, GunLight, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bLighterLight, LighterLight, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bAttackOn, AttackOn, GET_C_REF, GET_REF)
	GETSET_EX2(STATE_PLAYER, m_ePlayer_State, PlayerState, GET, SET)
	GETSET_EX2(STATE_RIGHTHAND, m_eRightHand_State, RightHandState, GET, SET)

	GETSET_EX2(_bool, m_bPlayerEquipGunState, PlayerGunState, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bPlayerAttackState, PlayerAttackState, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bPlayerPickUpState, PlayerPickUpState, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bPlayerDetect, PlayerDetect, GET_C_REF, GET_REF)
	GETSET_EX1(_bool, m_bRAttackMove, RAttackMove, GET_REF)
	GETSET_EX1(_bool, m_bLAttackMove, LAttackMove, GET_REF)
	GETSET_EX2(_bool, m_bPlayerHitState, PlayerHit, GET_C_REF, GET_REF)

	GETSET_EX2(_bool, m_bFade, SceneFade, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bMagicBottle, MagicBottle, GET_C_REF, GET_REF)

private:
	GAUGE<_float>		m_fHP;					// 플레이어 hp
	GAUGE<_float>		m_fStamina;				// 달리기, 공격 등에서 소모되는 스테미나 
	_bool				m_bGunLight;			// 총쏠때 켜지는 조명 블
	_bool				m_bLighterLight;		// 손으로 드는 라이터 

	_bool				m_bPlayerEquipGunState;	// 플레이어가 총을쥐고있을때
	_bool				m_bPlayerAttackState;	// 플레이어가 공격할때 (주먹, 장비, 발차기 등)
	_bool				m_bPlayerPickUpState;	// 들것 들고있을때 - 시체 
	_bool				m_bPlayerDetect;		// 플레이어 일점범위내 몬스터있을때 
	_bool				m_bAttackOn;
	_bool				m_bRAttackMove;
	_bool				m_bLAttackMove;
	
	

	STATE_PLAYER		m_ePlayer_State;
	STATE_RIGHTHAND		m_eRightHand_State;
	

	_bool				m_bPlayerHitState; //소영추가 
	_bool				m_bMagicBottle;
	_bool				m_bFade; 
};

