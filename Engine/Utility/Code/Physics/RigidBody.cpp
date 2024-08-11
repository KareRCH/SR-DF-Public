#include "RigidBody.h"

#include "CollisionPrimitive.h"


void FRigidBody::CalculateDerivedData()
{
	// 회전 쿼터니언 정규화
	qtOrientation.Normalise();

	// 몸체에 대한 트랜스폼 계산
	_CalculateTransformMatrix(matTransform, vPosition, qtOrientation);

	// 월드 스페이스에서 관성 텐서를 계산
	_TransformInertiaTensor(matInverseInertiaTensorWorld, qtOrientation,
								matInverseInertiaTensor, matTransform);
}

void FRigidBody::Integrate(const Real& fDuration)
{
	if (eBodyType == ERIGID_BODY_TYPE::STATIC) return;					// STATIC이면 힘을 부과하지 않는다.
	if (eBodyType == ERIGID_BODY_TYPE::DYNAMIC && !bIsAwake) return;	// DYNAMIC에 대해서만 재운다.

	// 선형 가속 합을 가속에 더하기
	vLastFrameAcceleration = vAcceleration;
	vLastFrameAcceleration.Add_ScaledVector(vForceAccum, fInverseMass);

	// 각 가속도를 계산
	FVector3 vAngularAcceleration = matInverseInertiaTensorWorld.Transform(vTorqueAccum);

	// 속도 조절
	// 선형 속도를 가속도와 충격으로부터 업데이트 한다.
	vVelocity.Add_ScaledVector(vLastFrameAcceleration, fDuration);

	// 각 속도를 가속도와 충격으로부터 업데이트 한다.
	vRotation.Add_ScaledVector(vAngularAcceleration, fDuration);

	// 드래그 값을 부과한다.
	vVelocity *= real_pow(fLinearDamping, fDuration);
	vRotation *= real_pow(fAngularDamping, fDuration);

	// 위치를 조절한다.
	// 선형 위치를 업데이트 한다.
	vPosition.Add_ScaledVector(vVelocity, fDuration);

	// 각 위치를 업데이트 한다.
	qtOrientation.Add_ScaledVector(vRotation, fDuration);

	// 정위를 정규화, 새로운 행렬을 업데이트한다.
	// 위치, 정위
	CalculateDerivedData();

	// 힘 합산기들을 정리한다.
	Clear_Accumulators();

	// 운동 에너지 저장을 업데이트, 몸체를 재울 수 있으면 재운다.
	if (eBodyType == ERIGID_BODY_TYPE::DYNAMIC && bCanSleep)
	{
		// 각 벡터들을 제곱한 값
		Real fCurrentMotion = vVelocity.DotProduct(vVelocity) + vRotation.DotProduct(vRotation);

		// 움직이고 있는지에 대한 평가치를 계산
		Real fBias = real_pow(0.5, fDuration);
		fMotion = (fBias * fMotion) + ((1 - fBias) * fCurrentMotion);

		// 오차와 계산해서 깨울지 재울지 결정
		//if (fMotion < fSleepEpsilon) { Set_Awake(false); }
		//else if (fMotion > 10 * fSleepEpsilon) { fMotion = 10 * fSleepEpsilon; }
	}
}
