#pragma once
#include "AceUnit.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "Player.h"

#include "Effect_HitBlood.h"
#include "Effect_HitPow.h"
#include "Effect_HitDust.h"
#include "Effect_FallingDust.h"
#include "Effect_DazeSwirl.h"
#include "UI_MonsterHP.h"

typedef struct tag_Monster_Info
{
	_float		fAttackDistance;					// ���� �þ߻�Ÿ�
	_float		fAttackFov;							// ���� �þ� ���� �ݰ� (90 = �þ߰��� 180��)

	_float		fConsider = 0.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ (���ҿ���)
	_float		fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	_float		fAwareness = 0;						// �ǽɰ����� ���� 
	_float		fMaxAwareness = 15.f;				// �ǽɰ����� max -> �߰����� ���� 

	_int		iDazedHP = 25;						// ���� �����ϴ� hp

	_vec3		vPatrolPointZero;					//�ʱ� ��������Ʈ 

}MONSTER;

typedef struct tag_MonsterSound_Info
{
	_int		m_iHpSection;		// ���������� ���� ���� ���� - 20���� ���������� 

	_float		m_fTalkAge;			// ���ϴ� �ð� - ����
	_float		m_fTalkLife;		// ���ϴ½ð� - ���� 
	_float		m_fTalkReapeat;		// ���ϴ½ð�  - �ݺ�Ƚ�� ī��Ʈ 

	_float		m_fSoundVolume;		// ���� ����ϴ� ���� 
	_float		m_fSoundEffectVolume; //����Ʈ �������

	_bool		m_bSoundOnce; // �ѹ��� ����Ǿ���Ҷ� 
	_bool		m_bSoundCheck; 

}MonsterSound;

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CAceMonster : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceMonster)

	PUBLIC enum class Dir {EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, NORTH, NORTHEAST, DIR_END };
protected:
	explicit CAceMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceMonster(const CAceMonster& rhs);
	virtual ~CAceMonster();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() PURE;
	virtual void		Free() override;

protected:
	STATE_RIGHTHAND				ePlayerRighthand;	// �浹�� ���¸ӽ� ���ư������� �÷��̾� ������ �ؽ��� ����뵵 
	FCollisionSphere*			pSphereShape;			// ���� �浹ü = boxŸ��
	MonsterSound				m_tSound;			// ���� �����ϱ����� ����ü 
	MONSTER						m_tStat;			// ������ ���� ���� ���� ����ü
	FRAME						m_tFrame;			// ������ �� ���� �� �ð����� ����ü 
	_vec3						vPlayerPos;			//�÷��̾� ��ġ���� �׻� �����ϴ� vec3
	_tchar						debugString[100];	// ����׿� sting 
	_bool						m_bDeadState;		// ��������
	_bool						m_bDazedState;		// ���ϴ� ���߿� ����°� �뵵 
	_bool						m_bDazeToHeal;		// ����� daze���̻� ����x
	_bool						m_bCollisionOn;     // ���� �浹�ѻ��¸� �ǹ� 
	Dir							m_eDirection;
	_float						VoiceDistance();
	_float						m_ftheta;
	_vec3						vDirect; //Go Home���� ���� 
	_float						m_fDistWeight;

protected:
	CRcBufferComp*				m_pBufferComp = nullptr; 
	CTextureComponent*			m_pTextureComp = nullptr;
	CColliderComponent*			m_pColliderComp = nullptr;
	CTransformComponent*		m_pTransformComp = nullptr;
	CCalculatorComponent*		m_pCalculatorComp = nullptr;
	CTransformComponent*		m_pPlayerTransformcomp = nullptr;

protected:
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster; // ������� 
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;  // ������� 

public: // ���� �߰� : ���� ���� Get,Set
	PLAYER_ATTACK_STATE m_ePlayer_AttackState;
	GETSET_EX2(GAUGE<_float>, m_gHp, MonsterHP, GET, SET)   // ���� HP 
	GETSET_EX2(PLAYER_ATTACK_STATE, m_ePlayer_AttackState, Player_AttackState, GET, SET)   // �ǰݴ��� ������ ���� (ex : ����ä�� ����, ���� ����, 2���� ����...���)
	
	GETSET_EX2(_float, m_tStat.fAwareness, Awareness, GET, SET)
	GETSET_EX2(_bool, m_bDazedState, IsMonsterDazed, GET, SET)   
	GETSET_EX2(_bool, m_bDeadState, IsMonsterDeath, GET, SET)   
	GETSET_EX2(_vec3, m_vPos, MonsterPos, GET, SET)
		
public:
	_float						Calc_Distance();
	_bool						Detect_Player();
	HRESULT						Get_PlayerPos();
	void						Height_On_Terrain(_float height);
	void						Billboard(const _float& fTimeDelta); 
	_bool						m_bPlayerAttakBool; // �÷��̾� ���ݻ��� ������� �����;��� 
	_vec3						m_vPos;
	void						Add_BasicEffect(CGameObject* pOwner);
	void						Calc_Theta();

	//�����Լ� ����Ʈ - ��ų �����Ҷ� ����� 
public:
	//low~high ������ ������ return 
	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

	// Get_RandomFloat�� vector�� 
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	// center���� radius ���� "������ġ" out�� ���� 
	// radius�� ��� ����������. 
	void GetRandomPointInCircle(_vec3* out, _vec3* center, float radius)
	{
		_float angle = static_cast<_float>(rand()) / RAND_MAX * 2 * D3DX_PI;
		_float r = static_cast<_float>(rand()) / RAND_MAX * radius;

		// ����ǥ�� ���� ��ǥ�� ��ȯ
		out->x = center->x + r * cosf(angle);
		out->y = center->y;
		out->z = center->z + r * sinf(angle);
	}

	// Center���� radius��ŭ ��������ġ�� ���� �׸��鼭 ��ġ�ϰ� �ϴµ� number������ŭ �����. 
	// ex. number4, radius2�ϰ�� ��������2�� ���� �׸��� ���� ������������ ��ġ�ϰԵȴ�. 
	void GetCirclePos(_vec3* Out, _vec3* Center, _int _number, _float _radius)
	{
	    for (_int i = 0; i < _number; ++i)
	    {
	        _float angle = static_cast<_float>(i) / static_cast<_float>(_number) * D3DX_PI * 2.0f;
	        Out->x = Center->x + _radius * cosf(angle);
	        Out->y = Center->y;
	        Out->z = Center->z + _radius * sinf(angle);
	
	    }
	}

	_bool Random_variable(_int _winning)
	{
		//�ü�� �ܿ��� �����ϴ� ��¥ ���� - ��ǻ�Ͱ� �ֺ��� ȯ��� ������������ ��ȣ�ۿ��ϸ鼭 ����°�
		// �ǻ糭������ ������ �����ϴ� �ӵ��� �ſ� ������. 
		random_device rd;

		// �޸��� Ʈ�����Ͷ�� �˰����� ����ؼ� ���� ���� ������ �ʱ�ȭ
		mt19937 gen(rd());

		// 0 ���� 99 ���� ����Ȯ������ ����   (min, max)
		uniform_int_distribution<_int> iDistribution(0, 99);

		//������ ���� 
		_int RandomValue = iDistribution(gen);

		//�Ű������� 5�ϰ�� 5���� Ȯ���� true�� ��ȯ�ȴ�
		return (RandomValue <= _winning);
	}

};