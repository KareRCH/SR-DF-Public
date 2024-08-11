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

	// Get, Set �Լ� ����� 
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

	//�浹 ����׿�
public:
	LPD3DXMESH		m_pMesh = nullptr;

	//������
private:
	void	Update_InternalData(); // ������� �����͸� �ø���. 
	void	Update_BlackBoard(); // ������κ��� �����͸� �޾ƿ��¿뵵 

protected:  
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster; // ������ ���� 
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;	// ������ �÷��̾�

private:
	// �Ÿ�
	_float		m_fRunDistance = 6.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 5.5f;				// run~walk ���� = walk
	_float		m_fInchDistance = 1.5f;  

	// ��ġ ���� 
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	
	//����ġ on/off 
	_bool		m_bArrive = FALSE;
	_bool		m_AttackOnce = FALSE;
	_bool		m_bCollisionEnter = TRUE;
	_bool		m_bSecondFall = FALSE;
	// �ܺ�Ÿ������ ���� ����
	enum class RECENT_COL {PLAYER, PLAYERATK, BOSSATK, RECEND_END};
	
	void MonsterDead();
	RECENT_COL		m_eRecentCol; // �÷��̾�1 �÷��̾����ü2 ������ų3 
	

#pragma region ���¸ӽ� enum����
	
public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, TAUNT, REST, CHASE, 		// ���	 
		RUN, WALK, STRAFING, INCHFORWARD, 			// �߰�
		NORMALATTACK, HEAVYATTACK, 					// ����
		HIT, FACEPUNCH, CROTCHHIT,					// �ǰ�
		DAZED, FALLING, CHOPPED, DEATH, HEADLESS,	// ����
		RECONNAISSANCE, GOHOME,						// ����

		//�̱��� ����Ʈ 
		JUMP
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, GOHOME, FALLING
	};

	// �ൿŰ
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
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//���� ����Ű 

#pragma endregion

#pragma region AI 

	// ��� 
	void AI_Idle(float fDeltaTime); // SOUTH �ٶ󺸴��� 
	void AI_Suspicious(float fDeltaTime); // �� ? �ǽ���
	void AI_Taunt(float fDeltaTime); // �ٰ��ͺ� �ϰ� ���߻��� 
	void AI_Chase(float fDeltaTime); // �Ÿ� üũ�ؼ� ���� ���� ���������� ������ ��꿪�� 
	void AI_Rest(float fDeltaTime); // ���ڸ����� ���Ÿ�

	// �߰� 
	void AI_Run(float fDeltaTime); // �޸��� 
	void AI_Walk(float fDeltaTime); // �ȱ�
	void AI_Strafing(float fDeltaTime); // �¿�� ���鼭 ��, �� ���� �ϳ��� �̵� 
	void AI_InchForward(float fDeltaTime); // ������ �� �ٰ����� ���� �ڼ� 
	
	// ����
	void AI_HeavyAttack(float fDeltaTime); // ������ 
	void AI_NormalATTACK(float fDeltaTime); // �Ϲ� ����
	
	// �ǰ�
	void AI_Hit(float fDeltaTime); // ���� ��Ʈ���� 
	void AI_FacePunch(float fDeltaTime); // �󱼿� �¾������ 
	void AI_Falling(float fDeltaTime); // ������ �¾������ 
	void AI_CrotchHit(float fDeltaTime); // �ϴ� �¾������  

	// ���� 
	void AI_Dazed(float fDeltaTime); // hp �����̻� ���� ���� 
	void AI_Death(float fDeltaTime); //ok // hp 0�λ��� 
	void AI_Chopped(float fDeltaTime); // �ڿ��� �÷��̾ �������������� Sleep���� �� 
	void AI_Headless(float fDeltaTime); //ok // �ѷ��� �Ӹ��� �¾������ 

	// ����
	void AI_Reconnaissance(float fDeltaTime); // �÷��̾� ���ļ� �ֺ� �����ϴ��� 
	void AI_GoHome(float fDeltaTime);		 // ������ġ�� ����ġ ������ 
	/*

	 Engine::Play_Sound(L"Enemy", L"Gray_FacePunchA.wav", SOUND_ENEMY_MONSTER, m_tSound.m_fSoundVolume);
	 
	  SOUND_ENEMY_MONSTER, SOUND_ENEMY_MONSTER_EFFECT,
 */
#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_NORMALATTACK + AI_HeavyAttack
	void Falling(float fDeltaTime);			// Gohome
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion

};

/* SPEED ����

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
����� ���� 

OutputDebugString(L"�� : �浹 ���� ����� 
OutputDebugString(L"�� : ���¸ӽ� ���� ����� 

swprintf_s(debugString, L"Brown - ���� Ȯ�� m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/