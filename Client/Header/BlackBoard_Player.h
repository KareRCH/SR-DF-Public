#pragma once

#include "BlackBoard.h"

class CPlayer;

/// <summary>
/// �̴ϼȶ������ ����ü
/// </summary>
struct FBlackBoard_PlayerInit
{
	CPlayer* pPlayer;
};


/// <summary>
/// �÷��̾� ���� ������
/// �÷��̾�� �ʿ��� ������� ���⿡ ������Ʈ ����
/// </summary>
class CBlackBoard_Player : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Player)
	// �÷��̾� ������ ���°�
	//enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	// �÷��̾� ���°�
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
	// �ʱ�ȭ�� ����ü�� ����� �Ű������� �ް� �Ѵ�.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_PlayerInit& tInit);

public:		// �÷��̾ ���� ������ ���� ���� 
	GETSET_EX1(CPlayer*, m_pPlayer, Player, GET)

private:
	CPlayer*			m_pPlayer = nullptr;

public:		// ������ ������ ������ �鶧�� �ش� �Լ��� ���� �����.
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
	GAUGE<_float>		m_fHP;					// �÷��̾� hp
	GAUGE<_float>		m_fStamina;				// �޸���, ���� ��� �Ҹ�Ǵ� ���׹̳� 
	_bool				m_bGunLight;			// �ѽ� ������ ���� ��
	_bool				m_bLighterLight;		// ������ ��� ������ 

	_bool				m_bPlayerEquipGunState;	// �÷��̾ �������������
	_bool				m_bPlayerAttackState;	// �÷��̾ �����Ҷ� (�ָ�, ���, ������ ��)
	_bool				m_bPlayerPickUpState;	// ��� ��������� - ��ü 
	_bool				m_bPlayerDetect;		// �÷��̾� ���������� ���������� 
	_bool				m_bAttackOn;
	_bool				m_bRAttackMove;
	_bool				m_bLAttackMove;
	
	

	STATE_PLAYER		m_ePlayer_State;
	STATE_RIGHTHAND		m_eRightHand_State;
	

	_bool				m_bPlayerHitState; //�ҿ��߰� 
	_bool				m_bMagicBottle;
	_bool				m_bFade; 
};

