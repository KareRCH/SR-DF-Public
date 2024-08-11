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

	// Get, Set �Լ� 
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

	//������ - Add BlackBoard���� �߰��� BoardName�� ���ε�, �ٿ�ε� �̸��� ��� ���ƾ��Ѵ�. 
private:
	void	Update_InternalData(); // ������� �����͸� �ø���. 
	void	Update_BlackBoard(); // ������κ��� �����͸� �޾ƿ��¿뵵 


#pragma region ���¸ӽ� enum����

public:
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, YOUDIE, TAUNT, CHASE, REST,			//�߰�
		RUN, WALK, KEEPEYE, SIDEWALK, FRIGHTEN, UPRIGHTRUN,		//����
		THROW, ATTACK,   HEAVYATTACK,							//����
		RECONNAISSANCE, GOHOME,									//����
		BLOCK,													//����
		CROTCHHIT, FACEPUNCH, FALLING, HIT,						//�ǰ�
		DAZED, DEATH, CHOPPED, HEADSHOT, HEADLESS				//���
		//�̱��� ����Ʈ 
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT {
		IDLE, APPROACH, SUDDENATTACK, SIDEMOVING, ATTACK,
		FALLING,
		GOHOME
	};

	//����Ű 
	enum class ACTION_KEY {
		IDLE, RUN, WALK, KEEPEYE, SIDEWALK,
		UPRIGHT, FRIGHTEN, BASIC_ATTACK, HEAVY_ATTACK,
		BACK,
		GOHOME
	};

#pragma endregion 

private:
	// ��Ÿ� 
	_float		m_fRunDistance = 8.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 7.5f;				// run~walk ���� = walk
	_float		m_fEyesOnYouDistance = 6.f;			// Eyes ~ Walk = ������ ���� 
	_float		m_fCloseToYouDistance = 2.f;		// Close ~ Eyes = 

	// ��ġ ���� - �������¿뵵 
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	
	//����ġ on/off 
	_bool		m_bArrive = FALSE; // ���� ���� ���� 
	_bool		m_AttackOnce = FALSE;
	_bool		m_bSecondFall = FALSE;

	// �ܺ�Ÿ������ ���� ����
	enum class RECENT_COL { PLAYER, PLAYERATK, BOSSATK, RECEND_END };

	void			MonsterDead();
	RECENT_COL		m_eRecentCol; // �÷��̾�1 �÷��̾����ü2 ������ų3 

public:
	using state_set_object = STATE_SET<STATE_OBJ, void(CGray*, float)>;
	GETSET_EX1(state_set_object, m_tState_Obj, StateObj, GET_REF)

private:
	//���¸ӽ� ��� 
	STATE_SET<STATE_OBJ, void(CGray*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CGray*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;

#pragma region AI 

	void AI_Idle(float fDeltaTime); // ó�� ���ִ� �뵵 
	void AI_Suspicious(float fDeltaTime); // ������ �߰��� 
	void AI_YouDie(float fDeltaTime); //����
	void AI_Taunt(float fDeltaTime); //����
	void AI_Chase(float fDeltaTime); //  �Ÿ��� ������ ����
	void AI_Rest(float fDeltaTime); // idle ready���� - �߰��߰� �ֱ�� 

	void AI_Run(float fDeltaTime); //������ �޷�����
	void AI_Walk(float fDeltaTime); //�Ϲ����� �ȱ� 
	void AI_KeepEye(float fDeltaTime); //�÷��̾� �ֽ���ä�� �齺�ǹ�� ������ 
	void AI_SideWalk(float fDeltaTime); //�÷��̾� �ֽ���ä�� ������ ���� - ���� ������ walk 
	void AI_Frighten(float fDeltaTime); //�����ؼ� ���� - �����ϴ°� ���⿡  ������ �ٷ� ��Ÿ�Ǿ���ҵ�
	void AI_UpRightRun(float fDeltaTime); //������ ��� �����Ϸ� �޷����»���

	void AI_Throw(float fDeltaTime); //������ ���� 
	void AI_Attack(float fDeltaTime); // ������ ��� �밢������ �� �����°�
	void AI_HeavyAttack(float fDeltaTime); //������ - ������ ���Ž�

	void AI_Reconnaissance(float fDeltaTime); // �÷��̾� ���ļ� �ֺ� �����ϴ��� 
	void AI_GoHome(float fDeltaTime); // ������ ��� �밢������ �� �����°�
	
	void AI_Block(float fDeltaTime); //�÷��̾ �����ϸ� ������

	void AI_CrotchHit( float fDeltaTime); // �ɱ� + �ǰ�
	void AI_FacePunch(float fDeltaTime); //�� �ǰ�
	void AI_Falling(float fDeltaTime); //�������� �Ͼ�°�
	void AI_Hit(float fDeltaTime); //������ ���ϰ� �Ǹ� �����»���
	
	void AI_Dazed(float fDeltaTime); //������ ���ϰ� �Ǹ� �����»���
	void AI_Death(float fDeltaTime); //�Ϲݰ������� �״»��� 
	void AI_Chopped(float fDeltaTime); //�ڵ��� ������ �ճ�ġ�� -> sleep 
	void AI_HeadShot(float fDeltaTime); // �ָ��̳� ��Ÿ ����� ��弦
	void AI_Headless(float fDeltaTime); //������ ������
	
	/*
	
	Engine::Play_Sound(L"Enemy", L"Gray_FacePunchA.wav", SOUND_ENEMY_MONSTER, m_tSound.m_fSoundVolume);
	 
	 SOUND_ENEMY_MONSTER, SOUND_ENEMY_MONSTER_EFFECT, 
	 */
#pragma endregion

#pragma region AI �ൿ 

	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime); // RUN + WALK 
	void SuddenAttack(float fDeltaTime);  // UPRIGHT + FRIGHTEN
	void SideMoving(float fDeltaTime); // EKKPEYE + SIDEWALK
	void Attack(float fDeltaTime); // BASIC ATTACK + HEAVY ATTACK
	void GoHome(float fDeltaTime); // GOHOME 
	void Falling(float fDeltaTime); // GOHOME 

#pragma endregion
};

/* SPEED ����

Speed 1 
Speed 2 
Speed 3 = GoHome,KEEP EYE
Speed 4 = walk, SideWalk
Speed 5 = UpRight
Speed 6 = run, frighten
Speed 7

swprintf_s(debugString, L"Gray - ���� Ȯ�� m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/