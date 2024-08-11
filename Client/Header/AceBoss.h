#pragma once
#include "AceMonster.h"
#include "BossLight.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"

#include "Serialize_BaseClass.h"

//���� ��� 
#include "MonsterPunch.h"

//��ų �� ����Ʈ ��� 
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
	_float INTRO_WalkDist			= 18.f; // 15�ʰ�			= walk
	_float INTRO_ShootDist			= 15.f; // 2 �ʰ� 15 ����    = Shoot
	_float INTRO_CloseAtkDist		= 2.f;	// 2 ����			= CloseAtk

	// Phase1 
	_float Ph1_RunDist				= 13.f; // 13 �ʰ�			= run
	_float Ph1_ShootDist			= 5.f;	// 5 �ʰ� ~ 13����	= shoot
	_float Ph1_RollDist				= 5.f;	// 2�ʰ� 5����		= roll + shoot
	_float Ph1_CloseAtkDist			= 2.f;	// 2 ����			= CloseAtk

	// Phase2
	_float Ph2_RunDist				= 15.f;	// 15 �ʰ�  			= run 
	_float Ph2_SideReadyDist		= 12.f;	// 12 �̻� 15 �̸�	= SideReady -> Shoot
	_float Ph2_RollDist				= 8.f;	//  8 �̻� 12 �̸�	= Roll + Shoot
	_float Ph2_ShootDist			= 8.f;	//  2 �̻� 8�̸�		= Shoot
	_float Ph2_CloseAtkDist			= 2.f;	//  2�̸�			= PreAtk -> CloseAtk
	
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
	
	// �浹 
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;
	
	//������ - Add BlackBoard���� �߰��� BoardName�� ���ε�, �ٿ�ε� �̸��� ��� ���ƾ��Ѵ�. 
private:
	void	Update_InternalData(); // ������� �����͸� �ø���. 
	void	Update_BlackBoard(); // ������κ��� �����͸� �޾ƿ��¿뵵 

private:
	// ���� 
	MonsterPhase	m_ePhase;
	DISTANCE		m_tDistance;

	_vec3			vDir; //�������� ���Ϳ뵵 
	_float			m_fTriggerHP;
	_bool			m_bPhaseStart = false;
	_float			m_fSideAge = 0.f;
	_float			m_fSideTime = 1.5f;
	_float			m_fAwareness;

	_float			m_fTalk;
	_float			m_fTalkLife;
	_float			m_fTalkReapeat;

	//���� ��Ʈ�� 
	_bool			m_AttackOnce = false;
	_bool			m_bBuffActive = false;
	_bool			m_bShitTrigger = false;
	_bool			m_bCollisionOn = false;
	_bool			m_bMakeGun = false;

	// ������� - ������ ���� 
	_bool m_bLightOn = FALSE;
	void LightControl(const _float& fTimeDelta);

#pragma region ���¸ӽ� enum����

public:
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, REST, CHASE,					// ���	  (SUS = BACKIDLE)
		RELOADING, RUN, WALK,							// �߰�
		
		PRE_ATTACK, SIDE_READY, ROLL, SHOUT,			// ���� 
		CLOSEATTACK,  SHOOTING, HEAVYSHOOT,				// ����
		PHASE1_INSTALL, PHASE2_INSTALL, PHASE2_BUFF,	//��ų����
		FALLING_STONE, ENERGY_BALL,						//��ų����
		RED_THUNDER, ROUND_FIRE,						//��ų����
		
		HIT, DAZED, FACEPUNCH, FALLING, CROTCHHIT,		// �ǰ�
		DEATH											// ����
		// ������ ������ ��ü�� �þ߹����̹Ƿ� ���������. 
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT {
		IDLE, 
		APPROACH,			// RUN + WALK 
		MOVING,	FALLING,		// ROLL
		ATTACK,				// CLOSEATTACK
		SHOOT,				//
		INSTALL, BUFF,		// ������ 1ȸ�� ��ų 
		SKILL_STONE, SKILL_ENERGYBALL,
		SKILL_THUNDER, SKILL_FIRE
	};

	// �׼�Ű
	enum class ACTION_KEY {
		IDLE,
		RUN, WALK, ROLL, FALLING,
		CLOSEATK,
		ATTACK,  // ??������?? 
		SHOOT,
		SKILL_LASER, SKILL_STONE, SKILL_ENERGYBALL,
		SKILL_BUFF, SKILL_THUNDER, SKILL_FIRE //��ų
	};

private:
	STATE_SET<STATE_OBJ, void(CAceBoss*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CAceBoss*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//���� ����Ű 

#pragma endregion

#pragma region AI 

	// ��� 
	void AI_Idle(float fDeltaTime); // SOUTH �ٶ󺸴��� 
	void AI_Suspicious(float fDeltaTime); // �� ? �ǽ���
	void AI_Rest(float fDeltaTime); // ���ڸ����� ���Ÿ�
	void AI_Chase(float fDeltaTime); // �Ÿ� üũ�ؼ� ���� ���� ���������� ������ ��꿪�� 

	// �߰� 
	void AI_Run(float fDeltaTime); // �޸��� 
	void AI_Walk(float fDeltaTime); // �ȱ�
	void AI_Reloading(float fDeltaTime); // �Ѿ� ������

	//����
	void AI_PreAttack(float fDeltaTime); // �������� ����  
	void AI_SideReady(float fDeltaTime); // ���� ������ �������� ���� 
	void AI_Roll(float fDeltaTime); // ������ ������ 

	// ����
	void AI_CloseAttack(float fDeltaTime); // �������� - ���Ӹ������� �ĸ���  
	void AI_Shoot(float fDeltaTime); // ������ ��°��� 
	void AI_HeavyShoot(float fDeltaTime); // ������ ��°��� 
	
	void AI_InstallPh1(float fDeltaTime); // 1���� ��ġ��
	void AI_InstallPh2(float fDeltaTime); // 2���� ��ġ��
	void AI_SkillBuff(float fDeltaTime); // 2���� �⺻���� ���� - Shout 
	
	void AI_SkillStone(float fDeltaTime); // 1���� ��ų - stone
	void AI_SkillEnergyBall(float fDeltaTime); // 1���� ��ų - ball 
	void AI_SkillThunder(float fDeltaTime); // 2���� ��ų
	void AI_SkillFire(float fDeltaTime); // 2���� ��ų
	
	// �ǰ�
	void AI_Hit(float fDeltaTime); // ���� ��Ʈ���� 
	void AI_Dazed(float fDeltaTime); // hp �����̻� ���� ���� 
	void AI_Falling(float fDeltaTime); // ������ �¾������ 
	void AI_FacePunch(float fDeltaTime); // �󱼿� �¾������ 
	void AI_CrotchHit(float fDeltaTime); // �ϴܰ���  

	// ���� 
	void AI_Death(float fDeltaTime); //ok // hp 0�λ��� 
	//Engine::Play_Sound(L"Enemy", L"_Death.wav", SOUND_ENEMY, m_tSound.m_fSoundVolume);

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// ROLL
	void Attack(float fDeltaTime);			// CloseATK
	void Shoot(float fDeltaTime);			// Shoot
	void Falling(float fDeltaTime);		    // Falling	

	void LaserInstall(float fDeltaTime);	// ������ ��ġ
	void BuffActive(float fDeltaTime);		// ���� �߰�  

	void SkillStone(float fDeltaTime);		// ��ų  
	void SkillEnergyBall(float fDeltaTime);	// ��ų  
	void SkillThunder(float fDeltaTime);	// ��ų  
	void SkillFire(float fDeltaTime);		// ��ų  


#pragma endregion

};


/* ��ų ���ݷ� ���� 
Falling Stone = 3
Fire = 4
EnergyBall = 4
Thunder = 6

�Ϲ� Atk = 3
Heavy Atk = 5
*/


/* SPEED ����

����� ����

OutputDebugString(L"�� : �浹 ���� �����
OutputDebugString(L"�� : ���¸ӽ� ���� �����
OutputDebugString(L"�ڡڡڡڡ�  �߿� ����׶��� \n");

swprintf_s(debugString, L"Boss - ���� Ȯ�� m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

10���� 
phase1 = energyball
phase2 = thunder 


*/