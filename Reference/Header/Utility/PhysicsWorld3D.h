#pragma once

#include "Base.h"
#include "Engine_Define.h"


#include "RigidBody.h"
#include "CollisionPrimitive.h"
#include "Contact.h"

BEGIN(Engine)

using list_collide_test = list<pair<FCollisionPrimitive*, FContact>>;

/// <summary>
/// ���� ���μ����� ����Ǵ� 3D ����
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

	// �ܼ� ���� �浹 �׽�Ʈ
	// �浹�� ��ü�� ���� ���� �浹�� ��ü�� ��ȯ. ���� �����ѵ����� ��.
	FCollisionPrimitive*		Test_Contact(FCollisionPrimitive* const pCollision);
	// �浹�� ��� �浹ü ��ȯ. �ַ� ���� ���̴� ���. �پ��� �浹ü�� �浹���̸� �ѹ��� ó���� �����ϴ�.
	list_collide_test			Test_Contacts(FCollisionPrimitive* const pCollision);

public:
	void						Add_RigidBody(FRigidBody* pBody);
	void						Delete_RigidBody(FRigidBody* pBody);

private:
	_bool						m_bCalculateIterations;			// ��� �ݺ�

	list<FRigidBody*>			m_listBody;						// ��ü ���� ��Ʈ

	FContactResolver			m_ContactResolver;				// ���� �ذ���
	list<FContactGenerator*>	m_ConGenList;					// ���� �߻��� ����Ʈ
	FContact*					m_pContacts;					// �߰� ����ó���� �ʿ��� ��ü�� ���� ������

	_uint						m_iMaxContacts;					// �ִ� ���� ����

public:
	void						Pause_Simulation() { m_bIsPaused = true; }
	void						Play_Simulation() { m_bIsPaused = false; }

private:
	_bool						m_bIsPaused = false;
};

END