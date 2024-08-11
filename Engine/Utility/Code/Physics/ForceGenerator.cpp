#include "ForceGenerator.h"


#pragma region 힘 등록기

void FForceRegistry::Update_Forces(const Real fDuration)
{
    auto iter = vecRegistrations.begin();
    for (; iter != vecRegistrations.end(); iter++)
    {
        // 등록기에 등록된 모든 객체에 대해 적용된 힘에 대해 업데이트 한다.
        iter->pFGen->Update_Force(iter->pBody, fDuration);
    }
}

void FForceRegistry::Add(FRigidBody* pBody, FForceGenerator* pFGen)
{
    FForceRegistry::FForceRegistration registration;
    registration.pBody = pBody;
    registration.pFGen = pFGen;
    vecRegistrations.push_back(registration);
}

#pragma endregion



#pragma region 중력

FForce_Gravity::FForce_Gravity(const FVector3& vGravity)
    : vGravity(vGravity)
{
}

void FForce_Gravity::Update_Force(FRigidBody* pBody, const Real& fDuration)
{
    // Check that we do not have infinite mass
    if (!pBody->HasFiniteMass()) return;

    // Apply the mass-scaled force to the body
    pBody->Add_Force(vGravity * pBody->Get_Mass());
}

#pragma endregion






#pragma region 스프링

FForce_Spring::FForce_Spring(const FVector3& localConnectionPt, FRigidBody* other, const FVector3& otherConnectionPt,
                                Real springConstant, Real restLength)
    : vConnectionPoint(localConnectionPt), vOtherConnectionPoint(otherConnectionPt), pOther(other),
    fSpringConstant(springConstant), fRestLength(restLength)
{
}

void FForce_Spring::Update_Force(FRigidBody* pBody, const Real& fDuration)
{
    // Calculate the two ends in world space
    FVector3 lws = pBody->Get_PointInWorldSpace(vConnectionPoint);
    FVector3 ows = pBody->Get_PointInWorldSpace(vOtherConnectionPoint);

    // Calculate the vector of the spring
    FVector3 force = lws - ows;

    // Calculate the magnitude of the force
    Real magnitude = force.Magnitude();
    magnitude = real_abs(magnitude - fRestLength);
    magnitude *= fSpringConstant;

    // Calculate the final force and apply it
    force.Normalize();
    force *= -magnitude;
    pBody->Add_ForceAtPoint(force, lws);
}

#pragma endregion
