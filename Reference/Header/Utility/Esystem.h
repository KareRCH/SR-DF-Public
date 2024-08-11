#pragma once
#include "GameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

class CSphereColComp;
class CColliderComponent;


class ENGINE_DLL CEsystem : public CGameObject
{
	DERIVED_CLASS(CGameObject, CEsystem)

public:
	enum class EFFECTTYPE { PER_SEC, PER_TURN, BOTH_TYPE, TYPE_END};

public:
	explicit CEsystem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEsystem(const CEsystem& rhs);
	virtual ~CEsystem();

public:
	virtual HRESULT		Ready_GameObject(EFFECTTYPE pType);
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject();
	virtual void		Free();

protected:
	virtual void		Render_GameObject() PURE; //����Ʈ���� �޶����Ƿ� 
	
public:
	CGameObject* m_pTarget;
	CRcBufferComp* m_pBufferComp;
	CTextureComponent* m_pTextureComp;
	CTransformComponent* m_pTransformComp;
	CCalculatorComponent* m_pCalculatorComp;

	CColliderComponent* m_pColliderComp;
	CSphereColComp* m_pSphereComp;

	//����Ʈ ���� bool�뵵
public: 
	_bool m_bAwareness;

public:
	GETSET_EX2(CGameObject*, m_pTarget, GameObject, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
		
	// �浹
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET) 
	GETSET_EX2(CSphereColComp*, m_pSphereComp, SphereColComponent, GET, SET) 
		
	// ����Ʈ Ʈ���� 
	GETSET_EX2(_bool, m_bAwareness, Awareness, GET, SET)
protected:
	// ������ ��������� �ϴ� �� - Ready_GameObject���� �Ű������� ���ϱ� �־��ּ��� 
	// ����Ʈ�� �ð������� �������, ��ü������ �ݺ�Ƚ���� ������� �����ؾ���,
	// �ٸ�Ÿ������ ������ TYPE_END �� �����ؼ� ���� Set_Dead��Ȳ ������ּ��� 

	EFFECTTYPE m_eType;
	// PER_SEC = ª�� �ݺ��� ����Ʈ�ǰ�� ������ �����ð����ȸ� ����Ʈ�� ���ӵ�
	// PER_TURN = �ð����� ��ü �����Ӵ����� nȸ �ݺ����ȸ� ����Ʈ�� ���ӵ�
	// BOTHT_YPE =  ���� ������ �Ǵ°����� ����Ʈ ���� 
	// TYPE_END = �����°� �˾Ƽ� ������ּ��� 
	
	//����Ʈ =======================================================

	_float				m_fFrame;		// �����ӽ��� - 0���� 
	_float				m_fFrameEnd;	// ������ �� = �ؽ�ó���� �޾ƿ��� 
	_float				m_fFrameSpeed;	// ������ ���ư��� �ӵ� 
	_float				m_fCheck;		// ������ �ѹ����ư��� ī��Ʈ ���� 

	_float				m_fAge;			// ó�� ����Ʈ �������� ����(?)
	_float				m_fLifeTime;	// ����Ʈ ���ӽð�

	_float				m_fGoalTurn;	// ��ǥ ������ 
	_float				m_fCurTurn;		// �������� �ѹ� ������ ���� ī��Ʈ ���� 

	// ���� - ���������ؾ��ϴ� ����Ʈ �뵵 =============================

	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;

	//===============================================================

	// �ι�° �Ű������� ����° �Ű����� ������ ������ ù��°�� �����ϴ��Լ� 
	
	// Get_RandomVector(&outvector, &minimum vector, &maxvector)
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	// _float output = Get_RandomFloat(0.0f, 1.0f) : 0 ~1 ������ ���� �������� ��ȯ�� 
	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

	// [����] Client > GameObject > Particle > FireWork > ResetParticle() �Լ� ������ ������ 
	//===============================================================

};

END