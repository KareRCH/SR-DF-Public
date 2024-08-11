#pragma once

#include "Engine_Define.h"

#include "RigidBody.h"
#include "Contact.h"

BEGIN(Engine)

enum class ECOLLISION_TYPE
{
	SPHERE, BOX, CAPSULE, PLANE, LINE, RAY, TRIANGLE, OBB
};
using ECOLLISION = ECOLLISION_TYPE;

struct FBoundingBox
{
	FVector3 vMin;
	FVector3 vMax;
};

/// <summary>
/// �浹ü
/// ���������� ����� ������ Ŭ����
/// ���� �浹ó�� �� Ŭ������ ������� �� �Ļ� Ŭ������ ���� �����Ѵ�.
/// </summary>
class ENGINE_DLL FCollisionPrimitive
{
	THIS_CLASS(FCollisionPrimitive)

public:
	FCollisionPrimitive()
	{
		pBody = new FRigidBody();
		pBody->Set_Owner(this);
		m_dwCollisionLayer_Flag = 0UL;
		m_dwCollisionMask_Flag = 0UL;
	}
	FCollisionPrimitive(const FCollisionPrimitive& rhs)
		: m_dwCollisionLayer_Flag(rhs.m_dwCollisionLayer_Flag)
		, m_dwCollisionMask_Flag(rhs.m_dwCollisionMask_Flag)
		, eType(rhs.eType)
	{
		pBody = new FRigidBody();
		*pBody = *rhs.pBody;
		pBody->Set_Owner(this);
		//arr = nullptr;		// �̺�Ʈ �Լ��� �����ϸ� �ȵ�.
	}
	virtual ~FCollisionPrimitive() 
	{
		Safe_Delete(pBody);
	}
	
public:
	GETSET_EX2(void*, pOwner, Owner, GET, SET)

protected:
	void* pOwner = nullptr;

private:
	// ���ȼ� �ڵ�
	void CheckAnd_CreateBody() const
	{
		if (!pBody) { pBody = new FRigidBody(); }
	}
	void Set_BodyType(const ERIGID_BODY_TYPE eType)
	{
		CheckAnd_CreateBody();
		pBody->Set_BodyType(eType);
	}
	ERIGID_BODY_TYPE Get_BodyType()
	{
		CheckAnd_CreateBody();
		return pBody->Get_BodyType();
	}

public:
	mutable FRigidBody* pBody;				// ��ü ����
	
	FMatrix3x4	matOffset;					// ������ ���
	FBoundingBox	BoundingBox;	// ��ε� ������ �� �ٿ�� �ڽ�


public:
	virtual void Calculate_Shape() PURE;

public:
	void Calculate_Transform()
	{
		matTransform = pBody->Get_Transform() * matOffset;
	}

public:
	GETSET_EX2(FMatrix3x4, matTransform, Transform, GET_C_REF, GET_REF)
	// �� ������ Ʈ���������� ����
	FVector3 Get_Axis(_uint iIndex) const
	{
		return matTransform.Get_AxisVector(iIndex);
	}

	const FVector3 Get_Position() const
	{
		return matTransform.Get_PosVector();
	}
	void Set_Position(const FVector3 vPos)
	{
		CheckAnd_CreateBody();
		pBody->Set_Position(vPos);
	}

	const FVector3& Get_Rotation() const
	{
		CheckAnd_CreateBody();
		return pBody->Get_Rotation();
	}
	void Set_Rotation(const FVector3 vRot)
	{
		CheckAnd_CreateBody();
		pBody->Set_Rotation(vRot);
	}

	
	FVector3 Get_Scale() const
	{
		return matTransform.Get_ScaleVector();
	}

protected:
	FMatrix3x4	matTransform;		// Ʈ���� �� ���

public:
	GETSET_EX2(_ulong, m_dwCollisionLayer_Flag, CollisionLayer, GET_C_REF, SET_C)
	GETSET_EX2(_ulong, m_dwCollisionMask_Flag, CollisionMask, GET_C_REF, SET_C)

protected:
	_ulong				m_dwCollisionLayer_Flag;			// �ݸ��� ���̾�, �浹ü�� �����ϴ� ��
	_ulong				m_dwCollisionMask_Flag;				// �ݸ��� ����ũ, �浹ü�� �浹�ϰ� �;��ϴ� ��

public:
	GETSET_EX2(ECOLLISION, eType, Type, GET_C_REF, GET_REF)

protected:
	ECOLLISION	eType;				// Ÿ��

public:
	void Add_CollisionEvent(const function<void(void*, const FContact* const)>& fn) { fnEventHandler = fn; }
	void Handle_CollsionEvent(void* pDst, const FContact* const pContact) { if (fnEventHandler) fnEventHandler(pDst, pContact); }

protected:
	function<void(void*, const FContact* const)>	fnEventHandler;
};


/// <summary>
/// ��ü
/// </summary>
class ENGINE_DLL FCollisionSphere : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionSphere)

public:
	FCollisionSphere()
		: fRadius(0.5f)
	{
		eType = ECOLLISION::SPHERE;
	}
	FCollisionSphere(const FCollisionSphere& rhs)
		: Base(rhs)
		, fRadius(rhs.fRadius)
	{
	}
	virtual ~FCollisionSphere() {}

public:
	virtual void Calculate_Shape() override
	{
		fRadius = max(max(Get_Scale().x, Get_Scale().y), Get_Scale().z) * 0.5f;
		BoundingBox.vMin = Get_Position() - FVector3(fRadius, fRadius, fRadius);
		BoundingBox.vMax = Get_Position() + FVector3(fRadius, fRadius, fRadius);
	}

public:
	Real		fRadius;
};


/// <summary>
/// �ڽ�
/// </summary>
class ENGINE_DLL FCollisionBox : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionBox)
public:
	FCollisionBox()
		: vHalfSize(0.5f, 0.5f, 0.5f)
	{
		eType = ECOLLISION::BOX;
	}
	FCollisionBox(const FCollisionBox& rhs)
		: Base(rhs)
		, vHalfSize(rhs.vHalfSize)
	{
	}
	virtual ~FCollisionBox() {}

public:
	virtual void Calculate_Shape() override
	{
		vHalfSize = Get_Scale() * 0.5f;
		BoundingBox.vMin = Get_Position() - vHalfSize;
		BoundingBox.vMax = Get_Position() + vHalfSize;
	}

public:
	FVector3	vHalfSize;
};


/// <summary>
/// ĸ��
/// </summary>
class ENGINE_DLL FCollisionCapsule : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionCapsule)
public:
	FCollisionCapsule()
		: vDirHalfSize(0.f, 1.f, 0.f), fRadius(0.5f)
	{
		eType = ECOLLISION::CAPSULE;
	}
	FCollisionCapsule(const FCollisionCapsule& rhs)
		: Base(rhs)
		, vDirHalfSize(rhs.vDirHalfSize), fRadius(rhs.fRadius)
	{
	}
	virtual ~FCollisionCapsule() {}

public:
	virtual void Calculate_Shape() override
	{
		vDirHalfSize = FVector3(0.f, Get_Scale().y * 0.5f, 0.f);
		fRadius = max(Get_Scale().x, Get_Scale().z) * 0.5f;
		BoundingBox.vMin = Get_Position() - FVector3(fRadius, vDirHalfSize.y + fRadius, fRadius);
		BoundingBox.vMax = Get_Position() + FVector3(fRadius, vDirHalfSize.y + fRadius, fRadius);
	}

public:
	FVector3	vDirHalfSize;
	Real		fRadius;
};


/// <summary>
/// ���
/// ����� ��ü ������ ������ �ʴ´�.
/// </summary>
class ENGINE_DLL FCollisionPlane : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionPlane)
public:
	FCollisionPlane()
		: vDirection(0.f, 1.f, 0.f), fOffset(1.f)
	{
		eType = ECOLLISION::PLANE;
	}
	FCollisionPlane(const FCollisionPlane& rhs)
		: Base(rhs)
		, vDirection(rhs.vDirection), fOffset(rhs.fOffset)
	{
	}
	virtual ~FCollisionPlane() {}

public:
	virtual void Calculate_Shape() override
	{
		//vHalfSize = matOffset.Get_ScaleVector() * 0.5f;
	}

public:
	FVector3	vDirection;
	Real		fOffset;
};


/// <summary>
/// ��
/// </summary>
class ENGINE_DLL FCollisionLine : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionLine)
public:
	FCollisionLine()
		: vStart(0.f, 0.f, 0.f), vEnd(0.f, 1.f, 0.f)
	{
		eType = ECOLLISION::LINE;
	}
	FCollisionLine(const FCollisionLine& rhs)
		: Base(rhs)
		, vStart(rhs.vStart), vEnd(rhs.vEnd)
	{
	}
	virtual ~FCollisionLine() {}

public:
	virtual void Calculate_Shape() override
	{
		//vHalfSize = matOffset.Get_ScaleVector() * 0.5f;
	}

public:
	FVector3	vStart;
	FVector3	vEnd;
};

/// <summary>
/// ����
/// </summary>
class ENGINE_DLL FCollisionRay : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionRay)
public:
	FCollisionRay()
		: vOrigin(0.f, 0.f, 0.f), vDir(0.f, 1.f, 0.f)
	{
		eType = ECOLLISION::RAY;
	}
	FCollisionRay(const FCollisionRay& rhs)
		: Base(rhs)
		, vOrigin(rhs.vOrigin), vDir(rhs.vDir)
	{
	}
	virtual ~FCollisionRay() {}

public:
	virtual void Calculate_Shape() override
	{
		//vHalfSize = matOffset.Get_ScaleVector() * 0.5f;
	}

public:
	FVector3	vOrigin;
	FVector3	vDir;
};

/// <summary>
/// �ﰢ��
/// </summary>
class ENGINE_DLL FCollisionTriangle : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionTriangle)
public:
	FCollisionTriangle()
		: vA(-0.5f, 0.f, -0.5f), vB(-0.5f, 0.f, 0.5f), vC(0.5f, 0.f, 0.5f)
	{
		eType = ECOLLISION::TRIANGLE;
	}
	FCollisionTriangle(const FCollisionTriangle& rhs)
		: Base(rhs)
		, vA(rhs.vA), vB(rhs.vB), vC(rhs.vC)
	{
	}
	virtual ~FCollisionTriangle() {}

public:
	virtual void Calculate_Shape() override
	{
		//vHalfSize = matOffset.Get_ScaleVector() * 0.5f;
	}

public:
	FVector3	vA, vB, vC;
};

/// <summary>
/// �ﰢ��
/// </summary>
class ENGINE_DLL FCollisionOBB : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionOBB)
public:
	FCollisionOBB()
		: vHalfSize(0.5f, 0.5f, 0.5f)
	{
		eType = ECOLLISION::OBB;
	}
	FCollisionOBB(const FCollisionOBB& rhs)
		: Base(rhs)
		, vHalfSize(rhs.vHalfSize)
	{
	}
	virtual ~FCollisionOBB() {}

public:
	virtual void Calculate_Shape() override
	{
		vHalfSize = Get_Scale() * 0.5f;
		Real fLength = vHalfSize.Magnitude();
		BoundingBox.vMin = Get_Position() - FVector3(fLength, fLength, fLength);
		BoundingBox.vMax = Get_Position() + FVector3(fLength, fLength, fLength);
	}

public:
	FVector3	vHalfSize;
};

END