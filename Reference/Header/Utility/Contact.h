#pragma once

#include "Engine_Define.h"

#include "PhysicsCore.h"
#include "PhysicsPrecision.h"
#include "RigidBody.h"

BEGIN(Engine)


/// <summary>
/// ���� ����
/// </summary>
class ENGINE_DLL FContact
{
	THIS_CLASS(FContact)

public:
	FRigidBody* pBody[2];           // �浹 ��ü

	Real		fFriction;          // ���� ���
	Real		fRestitution;       // �ݹ� ���

	FVector3	vContactPoint;      // ��������
	FVector3	vContactNormal;     // ���˹���
    Real        fPenetration;       // ���� ����

protected:
    FMatrix3    matContactToWorld;
    FMatrix3    matContactVelocity;
    Real        fDesiredDeltaVelocity;
    FVector3    vRelativeContactPosition[2];

public:
    

    void Set_BodyData(FRigidBody* pSrcBody, FRigidBody* pDstBody,
        Real _fFriction, Real _fRestitution)
    {
        pBody[0] = pSrcBody;
        pBody[1] = pDstBody;
        fFriction = _fFriction;
        fRestitution = _fRestitution;
    }

    void Reverse_BodyData()
    {
        FRigidBody* pTempBody = pBody[0];
        pBody[0] = pBody[1];
        pBody[1] = pBody[0];
        vContactNormal = -vContactNormal;
    }
};





/// <summary>
/// ���� �ذ�
/// </summary>
class ENGINE_DLL FContactResolver
{
	THIS_CLASS(FContactResolver)

protected:
	_uint	iVelocityIterations;        // �ӵ� ���� �ݺ��ڰ���
	_uint	iPositionIterations;        // ��ġ ���� �ݺ��ڰ���
	Real	fVelocityEpsilon;			// �ӵ� ����
	Real	fPositionEpsilon;			// ��ġ ����

public:
	_uint	iVelocityIterationsUsed;    // ���� �ӵ� �ݺ���
	_uint	iPositionIterationsUsed;    // ���� ��ġ �ݺ���

private:
	bool	bValidSettings;				// ��ȿ�� ��������

public:
    FContactResolver() {}
    FContactResolver(_uint iIterations, Real fVelocityEpsilon = (Real)0.01, Real fPositionEpsilon = (Real)0.01);
    FContactResolver(_uint iVelocityIterations, _uint iPositionIterations, Real fVelocityEpsilon = (Real)0.01, Real fPositionEpsilon = (Real)0.01);
    
    // ��ȿ üũ
    bool IsValid()
    {
        return (iVelocityIterations > 0) &&
            (iPositionIterations > 0) &&
            (fVelocityEpsilon >= 0.0f) &&
            (fPositionEpsilon >= 0.0f);
    }

    // �ݺ� ����
    void Set_Iterations(_uint _iVelocityIterations, _uint _iPositionIterations)
    {
        iVelocityIterations = _iVelocityIterations;
        iPositionIterations = _iPositionIterations;
    }
    void Set_Iterations(_uint iIterations)
    {
        Set_Iterations(iIterations, iIterations);
    }
    // ���� ����
    void Set_Epsilon(Real _fVelocityEpsilon, Real _fPositionEpsilon)
    {
        fVelocityEpsilon = _fVelocityEpsilon;
        fPositionEpsilon = _fPositionEpsilon;
    }
    void ResolveContacts(FContact* pContactArray, const _uint iContactCount, const Real& fDuration)
    {
        if (iContactCount == 0) return;
        if (IsValid()) return;

        // ���� �غ�
        PrepareContacts(pContactArray, iContactCount, fDuration);

        // ��ġ ����
        AdjustPositions(pContactArray, iContactCount, fDuration);

        // �ӵ� ����
        AdjustVelocities(pContactArray, iContactCount, fDuration);
    }

protected:
    // ���� �غ�
    void PrepareContacts(FContact* pContactArray, _uint iContactCount, Real fDuration)
    {
        // ������ ����� ����� �κ����� �����ϰ� ����.
        FContact* pLastContact = pContactArray + iContactCount;
        for (FContact* pContact = pContactArray; pContact < pLastContact; pContact++)
        {
            //pContact->C
        }
    }
    // �ӵ�, ��ġ ����
    void AdjustVelocities(FContact* pContactArray, _uint iContacts, Real fDuration);
    void AdjustPositions(FContact* pContacts, _uint iContacts, Real fDuration);
};


/// <summary>
/// Ư���� �浹�� ���̴� �߻� Ŭ����
/// ���˿� ���� �߻���Ų��.
/// </summary>
class ENGINE_DLL FContactGenerator abstract
{
	THIS_CLASS(FContactGenerator)

public:
	virtual _uint Add_Contact(FContact* pContact, _uint iLimit) const PURE;
};

END