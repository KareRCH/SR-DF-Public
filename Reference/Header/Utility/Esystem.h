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
	virtual void		Render_GameObject() PURE; //이펙트마다 달라지므로 
	
public:
	CGameObject* m_pTarget;
	CRcBufferComp* m_pBufferComp;
	CTextureComponent* m_pTextureComp;
	CTransformComponent* m_pTransformComp;
	CCalculatorComponent* m_pCalculatorComp;

	CColliderComponent* m_pColliderComp;
	CSphereColComp* m_pSphereComp;

	//이펙트 수행 bool용도
public: 
	_bool m_bAwareness;

public:
	GETSET_EX2(CGameObject*, m_pTarget, GameObject, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
		
	// 충돌
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET) 
	GETSET_EX2(CSphereColComp*, m_pSphereComp, SphereColComponent, GET, SET) 
		
	// 이펙트 트리거 
	GETSET_EX2(_bool, m_bAwareness, Awareness, GET, SET)
protected:
	// 무조건 셋팅해줘야 하는 값 - Ready_GameObject에서 매개변수로 들어가니까 넣어주세요 
	// 이펙트가 시간단위로 재생할지, 전체프레임 반복횟수로 재생할지 지정해야함,
	// 다른타입으로 만들경우 TYPE_END 로 설정해서 직접 Set_Dead상황 만들어주세요 

	EFFECTTYPE m_eType;
	// PER_SEC = 짧은 반복의 이펙트의경우 몇초의 생존시간동안만 이펙트가 지속됨
	// PER_TURN = 시간보다 전체 프레임단위의 n회 반복동안만 이펙트가 지속됨
	// BOTHT_YPE =  둘중 빠르게 되는것으로 이펙트 지속 
	// TYPE_END = 끝나는거 알아서 만들어주세요 
	
	//이펙트 =======================================================

	_float				m_fFrame;		// 프레임시작 - 0부터 
	_float				m_fFrameEnd;	// 프레임 끝 = 텍스처에서 받아오기 
	_float				m_fFrameSpeed;	// 프레임 돌아가는 속도 
	_float				m_fCheck;		// 프레임 한번돌아가면 카운트 증가 

	_float				m_fAge;			// 처음 이펙트 생겼을때 나이(?)
	_float				m_fLifeTime;	// 이펙트 지속시간

	_float				m_fGoalTurn;	// 목표 프레임 
	_float				m_fCurTurn;		// 프레임이 한번 끝까지 돌면 카운트 증가 

	// 변수 - 직교투영해야하는 이펙트 용도 =============================

	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;

	//===============================================================

	// 두번째 매개변수와 세번째 매개변수 사이의 값으로 첫번째에 리턴하는함수 
	
	// Get_RandomVector(&outvector, &minimum vector, &maxvector)
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	// _float output = Get_RandomFloat(0.0f, 1.0f) : 0 ~1 사이의 값을 무작위로 반환함 
	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

	// [예시] Client > GameObject > Particle > FireWork > ResetParticle() 함수 내에서 구현중 
	//===============================================================

};

END