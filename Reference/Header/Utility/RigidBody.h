#pragma once

#include "Engine_Define.h"
#include "PhysicsCore.h"

#include <memory.h>
#include <assert.h>

BEGIN(Engine)

class FCollisionPrimitive;

enum class ERIGID_BODY_TYPE
{
    // 질량 무한대, 움직이지 않는 지형 타입
    STATIC,
    // 물리 시뮬레이션에 종속적인 타입
    DYNAMIC,
    // 힘 발생기를 제외한 회전과 같은 힘에 영향을 받지 않는 타입, 플레이어 정의 물리 사용
    KINEMATIC
};

/// <summary>
/// 강체의 물리 속성을 가지는 클래스. 
/// F접두어는 언리얼에서 쓰이는 순수 클래스 접두어다. 
/// 여기서 모티브 받아 붙인 접두어이다.
/// 여기에서 F클래스는 구조체 처럼 쓰이거나 Base를 상속받지 않는 독립적인 형태의 클래스를 제작할 때 쓰인다.
/// Vector 클래스를 FloatVector, Integer, DoubleVector와 구분하기 위해 붙였다고 하는데...그렇다고 한다.
/// 멤버 변수 규칙은 m_를 붙이지 않습니다.
/// </summary>
class ENGINE_DLL FRigidBody
{
	THIS_CLASS(FRigidBody)

public:
    FRigidBody()
        : eBodyType(ERIGID_BODY_TYPE::STATIC), iID()
        , fInverseMass(Real()), matInverseInertiaTensor()
        , fLinearDamping(Real()), fAngularDamping(Real())
        , vForceAccum(), vTorqueAccum(), vAcceleration(), vLastFrameAcceleration()
        , vPosition(), vVelocity(), vRotation(), qtOrientation()
        , matTransform(), matInverseInertiaTensorWorld()
        , fMotion(Real()), bIsAwake(false), bCanSleep(false)
    {}
    FRigidBody(const FRigidBody& rhs)
        : eBodyType(rhs.eBodyType), iID(rhs.iID)
        , fInverseMass(rhs.fInverseMass), matInverseInertiaTensor(rhs.matInverseInertiaTensor)
        , fLinearDamping(rhs.fLinearDamping), fAngularDamping(rhs.fAngularDamping)
        , vForceAccum(rhs.vForceAccum), vTorqueAccum(rhs.vTorqueAccum), vAcceleration(rhs.vAcceleration), vLastFrameAcceleration(rhs.vLastFrameAcceleration)
        , vPosition(rhs.vPosition), vVelocity(rhs.vVelocity), vRotation(rhs.vRotation), qtOrientation(rhs.qtOrientation)
        , matTransform(rhs.matTransform), matInverseInertiaTensorWorld(rhs.matInverseInertiaTensorWorld)
        , fMotion(rhs.fMotion), bIsAwake(rhs.bIsAwake), bCanSleep(rhs.bCanSleep)
    {}
    ~FRigidBody() {}


private: // internal static
    static inline void _CheckInverseInertiaTensor(const FMatrix3& iitWorld)
    {
        // TODO: Perform a validity check in an assert.
    }

    static inline void _TransformInertiaTensor(FMatrix3& iitWorld,
        const FQuaternion& q, const FMatrix3& iitBody, const FMatrix3x4& rotmat)
    {
        Real t4 = rotmat.data[0] * iitBody.data[0] +
            rotmat.data[1] * iitBody.data[3] +
            rotmat.data[2] * iitBody.data[6];
        Real t9 = rotmat.data[0] * iitBody.data[1] +
            rotmat.data[1] * iitBody.data[4] +
            rotmat.data[2] * iitBody.data[7];
        Real t14 = rotmat.data[0] * iitBody.data[2] +
            rotmat.data[1] * iitBody.data[5] +
            rotmat.data[2] * iitBody.data[8];
        Real t28 = rotmat.data[4] * iitBody.data[0] +
            rotmat.data[5] * iitBody.data[3] +
            rotmat.data[6] * iitBody.data[6];
        Real t33 = rotmat.data[4] * iitBody.data[1] +
            rotmat.data[5] * iitBody.data[4] +
            rotmat.data[6] * iitBody.data[7];
        Real t38 = rotmat.data[4] * iitBody.data[2] +
            rotmat.data[5] * iitBody.data[5] +
            rotmat.data[6] * iitBody.data[8];
        Real t52 = rotmat.data[8] * iitBody.data[0] +
            rotmat.data[9] * iitBody.data[3] +
            rotmat.data[10] * iitBody.data[6];
        Real t57 = rotmat.data[8] * iitBody.data[1] +
            rotmat.data[9] * iitBody.data[4] +
            rotmat.data[10] * iitBody.data[7];
        Real t62 = rotmat.data[8] * iitBody.data[2] +
            rotmat.data[9] * iitBody.data[5] +
            rotmat.data[10] * iitBody.data[8];

        iitWorld.data[0] = t4 * rotmat.data[0] +
            t9 * rotmat.data[1] +
            t14 * rotmat.data[2];
        iitWorld.data[1] = t4 * rotmat.data[4] +
            t9 * rotmat.data[5] +
            t14 * rotmat.data[6];
        iitWorld.data[2] = t4 * rotmat.data[8] +
            t9 * rotmat.data[9] +
            t14 * rotmat.data[10];
        iitWorld.data[3] = t28 * rotmat.data[0] +
            t33 * rotmat.data[1] +
            t38 * rotmat.data[2];
        iitWorld.data[4] = t28 * rotmat.data[4] +
            t33 * rotmat.data[5] +
            t38 * rotmat.data[6];
        iitWorld.data[5] = t28 * rotmat.data[8] +
            t33 * rotmat.data[9] +
            t38 * rotmat.data[10];
        iitWorld.data[6] = t52 * rotmat.data[0] +
            t57 * rotmat.data[1] +
            t62 * rotmat.data[2];
        iitWorld.data[7] = t52 * rotmat.data[4] +
            t57 * rotmat.data[5] +
            t62 * rotmat.data[6];
        iitWorld.data[8] = t52 * rotmat.data[8] +
            t57 * rotmat.data[9] +
            t62 * rotmat.data[10];
    }

    static inline void _CalculateTransformMatrix(FMatrix3x4& transformMatrix,
        const FVector3& position, const FQuaternion& orientation)
    {
        transformMatrix.data[0] = 1 - 2 * orientation.j * orientation.j -
            2 * orientation.k * orientation.k;
        transformMatrix.data[1] = 2 * orientation.i * orientation.j -
            2 * orientation.r * orientation.k;
        transformMatrix.data[2] = 2 * orientation.i * orientation.k +
            2 * orientation.r * orientation.j;
        transformMatrix.data[3] = position.x;

        transformMatrix.data[4] = 2 * orientation.i * orientation.j +
            2 * orientation.r * orientation.k;
        transformMatrix.data[5] = 1 - 2 * orientation.i * orientation.i -
            2 * orientation.k * orientation.k;
        transformMatrix.data[6] = 2 * orientation.j * orientation.k -
            2 * orientation.r * orientation.i;
        transformMatrix.data[7] = position.y;

        transformMatrix.data[8] = 2 * orientation.i * orientation.k -
            2 * orientation.r * orientation.j;
        transformMatrix.data[9] = 2 * orientation.j * orientation.k +
            2 * orientation.r * orientation.i;
        transformMatrix.data[10] = 1 - 2 * orientation.i * orientation.i -
            2 * orientation.j * orientation.j;
        transformMatrix.data[11] = position.z;
    }

public:
    // 적분기
    void CalculateDerivedData();
    void Integrate(const Real& fDuration);

public:
    // 질량 함수
    inline void Set_Mass(const Real fMass)
    {
        assert(fMass != 0);
        fInverseMass = ((Real)1.0) / fMass;
    }
    inline Real Get_Mass() const
    {
        if (fInverseMass == 0) {
            return REAL_MAX;
        }
        else {
            return ((Real)1.0) / fInverseMass;
        }
    }
    inline bool HasFiniteMass() const
    {
        return fInverseMass >= 0.0f;
    }
    // 역질량 함수
    GETSET_EX2(Real, fInverseMass, InversMass, SET_C, GET)

    // 관성 함수
    inline void Set_InertiaTensor(const FMatrix3& matInertiaTensor)
    {
        matInverseInertiaTensor.Set_Inverse(matInertiaTensor);
        _CheckInverseInertiaTensor(matInverseInertiaTensor);
    }
    inline void Get_InertiaTensor(FMatrix3* matInertiaTensor) const
    {
        matInertiaTensor->Set_Inverse(matInverseInertiaTensor);
    }
    inline FMatrix3 Get_InertiaTensor() const
    {
        FMatrix3 it;
        Get_InertiaTensor(&it);
        return it;
    }
    inline void Get_InertiaTensorWorld(FMatrix3* matInertiaTensor) const
    {
        matInertiaTensor->Set_Inverse(matInverseInertiaTensor);
    }
    inline FMatrix3 Get_InertiaTensorWorld() const
    {
        return matInverseInertiaTensorWorld;
    }
    
    
    // 역관성 함수
    inline void Set_InverseInertiaTensor(const FMatrix3& matInverseInertiaTensor)
    {
        _CheckInverseInertiaTensor(matInverseInertiaTensor);
        this->matInverseInertiaTensor = matInverseInertiaTensor;
    }
    inline void Get_InverseInertiaTensor(FMatrix3* matInverseInertiaTensor) const
    {
        *matInverseInertiaTensor = this->matInverseInertiaTensor;
    }
    inline FMatrix3 Get_InverseInertiaTensor() const
    {
        return matInverseInertiaTensor;
    }
    // 역관성 월드 행렬 함수
    inline void Get_InverseInertiaTensorWorld(FMatrix3* matInverseInertiaTensor) const
    {
        *matInverseInertiaTensor = matInverseInertiaTensorWorld;
    }
    inline FMatrix3 Get_InverseInertiaTensorWorld() const
    {
        return matInverseInertiaTensorWorld;
    }


    // 감쇄 함수
    GETSET_EX2(Real, fLinearDamping, LinearDamping, SET_C, GET)
    GETSET_EX2(Real, fAngularDamping, AngularDamping, SET_C, GET)
    inline void Set_Damping(const Real _fLinearDamping, const Real _fAngularDamping)
    {
        fLinearDamping = _fLinearDamping;
        fAngularDamping = _fAngularDamping;
    }
    

    // 위치 함수
    GETSET_EX2(FVector3, vPosition, Position, SET_C_REF, GET)
    inline void Set_Position(const Real x, const Real y, const Real z)
    {
        vPosition.x = x;
        vPosition.y = y;
        vPosition.z = z;
    }
    inline void Get_Position(FVector3* _vPosition) const
    {
        *_vPosition = vPosition;
    }


    // 정위 함수
    GETSET_EX1(FQuaternion, qtOrientation, Orientation, GET)
    inline void Set_Orientation(const FQuaternion& orientation)
    {
        qtOrientation = orientation;
        qtOrientation.Normalise();          // 정규화 필수
    }
    inline void Set_Orientation(const Real r, const Real i, const Real j, const Real k)
    {
        qtOrientation.r = r;
        qtOrientation.i = i;
        qtOrientation.j = j;
        qtOrientation.k = k;
        qtOrientation.Normalise();          // 정규화 필수
    }
    inline void Get_Orientation(FQuaternion* orientation) const
    {
        *orientation = qtOrientation;
    }
    inline void Get_Orientation(FMatrix3* matrix) const
    {
        Get_Orientation(matrix->data);  // 아래함수에서 처리
    }
    inline void Get_Orientation(Real matrix[9]) const
    {
        // 3x4 -> 3x3
        matrix[0] = matTransform.data[0];
        matrix[1] = matTransform.data[1];
        matrix[2] = matTransform.data[2];

        matrix[3] = matTransform.data[4];
        matrix[4] = matTransform.data[5];
        matrix[5] = matTransform.data[6];

        matrix[6] = matTransform.data[8];
        matrix[7] = matTransform.data[9];
        matrix[8] = matTransform.data[10];
    }

    // 트랜스폼 함수
    GETSET_EX1(FMatrix3x4, matTransform, Transform, GET)
    inline void Get_Transform(FMatrix3x4* transform) const
    {
        memcpy(transform, &matTransform.data, sizeof(FMatrix3x4));
    }
    inline void Get_Transform(Real matrix[16]) const
    {
        // 3x4 -> 4x4
        memcpy(matrix, matTransform.data, sizeof(Real) * 12);
        matrix[12] = matrix[13] = matrix[14] = 0;
        matrix[15] = 1;
    }
    inline void Get_GLTransform(_float matrix[16]) const
    {
        matrix[0] = (float)matTransform.data[0];
        matrix[1] = (float)matTransform.data[4];
        matrix[2] = (float)matTransform.data[8];
        matrix[3] = 0;

        matrix[4] = (float)matTransform.data[1];
        matrix[5] = (float)matTransform.data[5];
        matrix[6] = (float)matTransform.data[9];
        matrix[7] = 0;

        matrix[8] = (float)matTransform.data[2];
        matrix[9] = (float)matTransform.data[6];
        matrix[10] = (float)matTransform.data[10];
        matrix[11] = 0;

        matrix[12] = (float)matTransform.data[3];
        matrix[13] = (float)matTransform.data[7];
        matrix[14] = (float)matTransform.data[11];
        matrix[15] = 1;
    }
    inline void Get_DXTransform(_float matrix[16]) const
    {
        matrix[0] = (float)matTransform.data[0];
        matrix[1] = (float)matTransform.data[1];
        matrix[2] = (float)matTransform.data[2];
        matrix[3] = 0;

        matrix[4] = (float)matTransform.data[4];
        matrix[5] = (float)matTransform.data[5];
        matrix[6] = (float)matTransform.data[6];
        matrix[7] = 0;

        matrix[8] = (float)matTransform.data[8];
        matrix[9] = (float)matTransform.data[9];
        matrix[10] = (float)matTransform.data[10];
        matrix[11] = 0;

        matrix[12] = (float)matTransform.data[3];
        matrix[13] = (float)matTransform.data[7];
        matrix[14] = (float)matTransform.data[11];
        matrix[15] = 1;
    }
    // 벡터에 대한 변환 함수
    FVector3 Get_PointInLocalSpace(const FVector3& vPoint) const
    {
        return matTransform.TransformInverse(vPoint);
    }
    FVector3 Get_PointInWorldSpace(const FVector3& vPoint) const
    {
        return matTransform.Transform(vPoint);
    }
    FVector3 Get_DirectionInLocalSpace(const FVector3& vDirection) const
    {
        return matTransform.TransformInverseDirection(vDirection);
    }
    FVector3 Get_DirectionInWorldSpace(const FVector3& vDirection) const
    {
        return matTransform.TransformDirection(vDirection);
    }

    // 속도 함수
    GETSET_EX2(FVector3, vVelocity, Velocity, SET_C_REF, GET)
    inline void Set_Velocity(const Real x, const Real y, const Real z)
    {
        vVelocity.x = x; vVelocity.y = y; vVelocity.z = z;
    }
    inline void Get_Velocity(FVector3* velocity) const
    {
        *velocity = vVelocity;
    }
    inline void Add_Velocity(const FVector3& vDeltaVelocity)
    {
        vVelocity += vDeltaVelocity;
    }


    // 회전 함수
    GETSET_EX2(FVector3, vRotation, Rotation, SET_C_REF, GET)
    inline void Set_Rotation(const Real x, const Real y, const Real z) 
    { vRotation.x = x; vRotation.y = y; vRotation.z = z; }
    inline void Get_Rotation(FVector3* rotation) const 
    { *rotation = vRotation; }
    inline void Add_Rotation(const FVector3& vDeltaRotation) 
    { vRotation += vDeltaRotation; }


    // 동면 함수
    inline bool Get_Awake() const { return bIsAwake; }
    inline void Set_Awake(const bool bWake = true)
    {
        if (bWake) {
            bIsAwake = true;

            // Add a bit of motion to avoid it falling asleep immediately.
            //fMotion = fSleepEpsilon * 2.0f;
        }
        else {
            bIsAwake = false;
            vVelocity.Clear();
            vRotation.Clear();
        }
    }
    inline bool Get_CanSleep() const { return bCanSleep; }
    inline void Set_CanSleep(const bool _bCanSleep = true)
    {
        bCanSleep = _bCanSleep;
        if (!_bCanSleep && !bIsAwake)
            Set_Awake();
    }


    // 힘 함수
    inline void Clear_Accumulators()
    {
        vForceAccum.Clear();
        vTorqueAccum.Clear();
    }
    inline void Add_Force(const FVector3& vForce)
    {
        vForceAccum += vForce;
        bIsAwake = true;
    }
    inline void Add_ForceAtPoint(const FVector3& vForce, const FVector3& vPoint)
    {
        FVector3 vPT = vPoint;
        vPT -= vPosition;

        vForceAccum += vForce;
        vTorqueAccum += vPT % vForce;   // 외적

        bIsAwake = true;
    }
    inline void Add_ForceAtBodyPoint(const FVector3& vForce, const FVector3& vPoint)
    {
        // 월드 좌표의 점을 얻어 힘 합산기에 적용한다.
        FVector3 vPT = Get_PointInWorldSpace(vPoint);
        Add_ForceAtPoint(vForce, vPT);
    }
    inline void Add_Torque(const FVector3& vTorque)
    {
        vTorqueAccum += vTorque;
        bIsAwake = true;
    }


    // 가속 함수
    GETSET_EX2(FVector3, vAcceleration, Acceleration, SET_C_REF, GET)
    inline void Set_Acceleration(const Real x, const Real y, const Real z)
    {
        vAcceleration.x = x; vAcceleration.y = y; vAcceleration.z = z;
    }
    inline void Get_Acceleration(FVector3* pvAcceleration) const
    {
        *pvAcceleration = vAcceleration;
    }
    // 이전 프레임 가속값 얻기
    inline void Get_LastFrameAcceleration(FVector3* linearAcceleration) const
    {
        *linearAcceleration = vLastFrameAcceleration;
    }
    inline FVector3 Get_LastFrameAcceleration() const
    {
        return vLastFrameAcceleration;
    }
    GETSET_EX2(ERIGID_BODY_TYPE, eBodyType, BodyType, GET_C_REF, SET_C)
    GETSET_EX2(unsigned long long, iID, ID, GET_C_REF, SET_C)

protected:
    ERIGID_BODY_TYPE                eBodyType;
    unsigned long long              iID;

protected:
    Real                            fInverseMass;				    // 역질량
	FMatrix3                        matInverseInertiaTensor;		// 역관성텐서

    Real                            fLinearDamping;                 // 등속이동 감쇄
    Real                            fAngularDamping;                // 각속도 감쇄
    FVector3                        vForceAccum;                    // 속도힘 합
    FVector3                        vTorqueAccum;                   // 회전힘 합
    FVector3                        vAcceleration;                  // 가속도
    FVector3                        vLastFrameAcceleration;         // 이전 프레임 가속도
    
    FVector3                        vPosition;                      // 위치
    FVector3                        vVelocity;                      // 속도
    FVector3                        vRotation;                      // 회전 속도
    FQuaternion                     qtOrientation;                  // 정위, 나중에 쿼터니온으로 바꿀 것
    

    FMatrix3                        matInverseInertiaTensorWorld;   // 3차원 행렬
    FMatrix3x4                      matTransform;                   // 4차원 행렬, 트랜스폼
    
    Real                            fMotion;                        // 모션
    _bool                           bIsAwake;                       // 힘 작용 업데이트 가능 여부
    _bool                           bCanSleep;                      // 앱실론 자동 연산 제외 기능

public:
    GETSET_EX2(void*, pOwner, Owner, GET, SET)

protected:
    void*           pOwner = nullptr;                // 충돌기하학 모양, 
};

END