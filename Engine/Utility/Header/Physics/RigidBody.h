#pragma once

#include "Engine_Define.h"
#include "PhysicsCore.h"

#include <memory.h>
#include <assert.h>

BEGIN(Engine)

class FCollisionPrimitive;

enum class ERIGID_BODY_TYPE
{
    // ���� ���Ѵ�, �������� �ʴ� ���� Ÿ��
    STATIC,
    // ���� �ùķ��̼ǿ� �������� Ÿ��
    DYNAMIC,
    // �� �߻��⸦ ������ ȸ���� ���� ���� ������ ���� �ʴ� Ÿ��, �÷��̾� ���� ���� ���
    KINEMATIC
};

/// <summary>
/// ��ü�� ���� �Ӽ��� ������ Ŭ����. 
/// F���ξ�� �𸮾󿡼� ���̴� ���� Ŭ���� ���ξ��. 
/// ���⼭ ��Ƽ�� �޾� ���� ���ξ��̴�.
/// ���⿡�� FŬ������ ����ü ó�� ���̰ų� Base�� ��ӹ��� �ʴ� �������� ������ Ŭ������ ������ �� ���δ�.
/// Vector Ŭ������ FloatVector, Integer, DoubleVector�� �����ϱ� ���� �ٿ��ٰ� �ϴµ�...�׷��ٰ� �Ѵ�.
/// ��� ���� ��Ģ�� m_�� ������ �ʽ��ϴ�.
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
    // ���б�
    void CalculateDerivedData();
    void Integrate(const Real& fDuration);

public:
    // ���� �Լ�
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
    // ������ �Լ�
    GETSET_EX2(Real, fInverseMass, InversMass, SET_C, GET)

    // ���� �Լ�
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
    
    
    // ������ �Լ�
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
    // ������ ���� ��� �Լ�
    inline void Get_InverseInertiaTensorWorld(FMatrix3* matInverseInertiaTensor) const
    {
        *matInverseInertiaTensor = matInverseInertiaTensorWorld;
    }
    inline FMatrix3 Get_InverseInertiaTensorWorld() const
    {
        return matInverseInertiaTensorWorld;
    }


    // ���� �Լ�
    GETSET_EX2(Real, fLinearDamping, LinearDamping, SET_C, GET)
    GETSET_EX2(Real, fAngularDamping, AngularDamping, SET_C, GET)
    inline void Set_Damping(const Real _fLinearDamping, const Real _fAngularDamping)
    {
        fLinearDamping = _fLinearDamping;
        fAngularDamping = _fAngularDamping;
    }
    

    // ��ġ �Լ�
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


    // ���� �Լ�
    GETSET_EX1(FQuaternion, qtOrientation, Orientation, GET)
    inline void Set_Orientation(const FQuaternion& orientation)
    {
        qtOrientation = orientation;
        qtOrientation.Normalise();          // ����ȭ �ʼ�
    }
    inline void Set_Orientation(const Real r, const Real i, const Real j, const Real k)
    {
        qtOrientation.r = r;
        qtOrientation.i = i;
        qtOrientation.j = j;
        qtOrientation.k = k;
        qtOrientation.Normalise();          // ����ȭ �ʼ�
    }
    inline void Get_Orientation(FQuaternion* orientation) const
    {
        *orientation = qtOrientation;
    }
    inline void Get_Orientation(FMatrix3* matrix) const
    {
        Get_Orientation(matrix->data);  // �Ʒ��Լ����� ó��
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

    // Ʈ������ �Լ�
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
    // ���Ϳ� ���� ��ȯ �Լ�
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

    // �ӵ� �Լ�
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


    // ȸ�� �Լ�
    GETSET_EX2(FVector3, vRotation, Rotation, SET_C_REF, GET)
    inline void Set_Rotation(const Real x, const Real y, const Real z) 
    { vRotation.x = x; vRotation.y = y; vRotation.z = z; }
    inline void Get_Rotation(FVector3* rotation) const 
    { *rotation = vRotation; }
    inline void Add_Rotation(const FVector3& vDeltaRotation) 
    { vRotation += vDeltaRotation; }


    // ���� �Լ�
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


    // �� �Լ�
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
        vTorqueAccum += vPT % vForce;   // ����

        bIsAwake = true;
    }
    inline void Add_ForceAtBodyPoint(const FVector3& vForce, const FVector3& vPoint)
    {
        // ���� ��ǥ�� ���� ��� �� �ջ�⿡ �����Ѵ�.
        FVector3 vPT = Get_PointInWorldSpace(vPoint);
        Add_ForceAtPoint(vForce, vPT);
    }
    inline void Add_Torque(const FVector3& vTorque)
    {
        vTorqueAccum += vTorque;
        bIsAwake = true;
    }


    // ���� �Լ�
    GETSET_EX2(FVector3, vAcceleration, Acceleration, SET_C_REF, GET)
    inline void Set_Acceleration(const Real x, const Real y, const Real z)
    {
        vAcceleration.x = x; vAcceleration.y = y; vAcceleration.z = z;
    }
    inline void Get_Acceleration(FVector3* pvAcceleration) const
    {
        *pvAcceleration = vAcceleration;
    }
    // ���� ������ ���Ӱ� ���
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
    Real                            fInverseMass;				    // ������
	FMatrix3                        matInverseInertiaTensor;		// �������ټ�

    Real                            fLinearDamping;                 // ����̵� ����
    Real                            fAngularDamping;                // ���ӵ� ����
    FVector3                        vForceAccum;                    // �ӵ��� ��
    FVector3                        vTorqueAccum;                   // ȸ���� ��
    FVector3                        vAcceleration;                  // ���ӵ�
    FVector3                        vLastFrameAcceleration;         // ���� ������ ���ӵ�
    
    FVector3                        vPosition;                      // ��ġ
    FVector3                        vVelocity;                      // �ӵ�
    FVector3                        vRotation;                      // ȸ�� �ӵ�
    FQuaternion                     qtOrientation;                  // ����, ���߿� ���ʹϿ����� �ٲ� ��
    

    FMatrix3                        matInverseInertiaTensorWorld;   // 3���� ���
    FMatrix3x4                      matTransform;                   // 4���� ���, Ʈ������
    
    Real                            fMotion;                        // ���
    _bool                           bIsAwake;                       // �� �ۿ� ������Ʈ ���� ����
    _bool                           bCanSleep;                      // �۽Ƿ� �ڵ� ���� ���� ���

public:
    GETSET_EX2(void*, pOwner, Owner, GET, SET)

protected:
    void*           pOwner = nullptr;                // �浹������ ���, 
};

END