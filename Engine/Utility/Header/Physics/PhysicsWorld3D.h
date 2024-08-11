#pragma once

#include "Base.h"
#include "Engine_Define.h"


#include "RigidBody.h"
#include "CollisionPrimitive.h"
#include "Contact.h"

BEGIN(Engine)

using list_collide_test = list<pair<FCollisionPrimitive*, FContact>>;

/// <summary>
/// 물리 프로세스가 진행되는 3D 공간
/// </summary>
class ENGINE_DLL CPhysicsWorld3D : public CBase
{
	DERIVED_CLASS(CBase, CPhysicsWorld3D)

private:
	explicit CPhysicsWorld3D();
	virtual ~CPhysicsWorld3D();

public:
	static CPhysicsWorld3D* Create(_uint iMaxContacts, _uint iIterations);

private:
	virtual		void	Free();


public:
	HRESULT						Ready_Physics(_uint iMaxContacts, _uint iIterations);
	void						StartFrame_Physics();
	_int						Update_Physics(const Real& fTimeDelta);
	_uint						Generate_Contacts();

	// 단순 물리 충돌 테스트
	// 충돌한 객체중 가장 먼저 충돌한 객체를 반환. 정말 간단한데서만 씀.
	FCollisionPrimitive*		Test_Contact(FCollisionPrimitive* const pCollision);
	// 충돌한 모든 충돌체 반환. 주로 많이 쓰이는 방식. 다양한 충돌체와 충돌중이면 한번에 처리가 가능하다.
	list_collide_test			Test_Contacts(FCollisionPrimitive* const pCollision);

public:
	void						Add_RigidBody(FRigidBody* pBody);
	void						Delete_RigidBody(FRigidBody* pBody);

private:
	_bool						m_bCalculateIterations;			// 계산 반복

	list<FRigidBody*>			m_listBody;						// 강체 정보 세트

	FContactResolver			m_ContactResolver;				// 접촉 해결자
	list<FContactGenerator*>	m_ConGenList;					// 접촉 발생기 리스트
	FContact*					m_pContacts;					// 추가 접촉처리가 필요한 객체에 대한 포인터

	_uint						m_iMaxContacts;					// 최대 접촉 개수

public:
	void						Pause_Simulation() { m_bIsPaused = true; }
	void						Play_Simulation() { m_bIsPaused = false; }

private:
	_bool						m_bIsPaused = false;
};

END