#pragma once

#include "Engine_Define.h"

#include "RigidBody.h"

BEGIN(Engine)

class FForceGenerator;

/// <summary>
/// �� ��ϱ�
/// </summary>
class ENGINE_DLL FForceRegistry
{
protected:
    struct FForceRegistration
    {
        FRigidBody* pBody;
        FForceGenerator* pFGen;
    };
    typedef vector<FForceRegistration> Registry;
    Registry vecRegistrations;

public:
    void Add(FRigidBody* pBody, FForceGenerator* pFGen);
    void Remove(FRigidBody* pBody, FForceGenerator* pFGen);
    void Clear();
    void Update_Forces(const Real fDuration);
};


/// <summary>
/// ���� ���� �߻���Ű�� Ŭ������ ����
/// </summary>
class ENGINE_DLL FForceGenerator abstract
{
	THIS_CLASS(FForceGenerator)

public:
	virtual void Update_Force(FRigidBody* pBody, const Real& fDuration) PURE;

};


/// <summary>
/// �߷� �� �߻���
/// </summary>
class ENGINE_DLL FForce_Gravity
{
	THIS_CLASS(FForce_Gravity)

private:
	FVector3 vGravity;

public:
	FForce_Gravity(const FVector3& gravity);

	virtual void Update_Force(FRigidBody* pBody, const Real& fDuration);
};


/// <summary>
/// ������ �� �߻���
/// </summary>
class ENGINE_DLL FForce_Spring
{
	THIS_CLASS(FForce_Spring)

private:
	FVector3 vConnectionPoint;		// ���� ����Ʈ
	FVector3 vOtherConnectionPoint;	// �ٸ� ��ü ���� ����Ʈ

	FRigidBody* pOther;				// �ٸ� ��ü

	Real		fSpringConstant;	// ������ ���
	Real		fRestLength;		// ���� ����, ���� ����

public:
	FForce_Spring(const FVector3& localConnectionPt, FRigidBody* other, const FVector3& otherConnectionPt,
						Real springConstant, Real restLength);

	virtual void Update_Force(FRigidBody* pBody, const Real& fDuration);
};

END