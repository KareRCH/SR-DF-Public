#pragma once

#include "Engine_Define.h"

#include "PhysicsCore.h"
#include "PhysicsPrecision.h"
#include "CollisionPrimitive.h"
#include "RigidBody.h"
#include "Contact.h"

BEGIN(Engine)

#define CMP(x, y) \
	(fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

/// <summary>
/// 각종 기하학적 모형들의 충돌테스트 클래스
/// static 클래스, CollsionData를 생성합니다.
/// </summary>
class ENGINE_DLL FIntersectTests
{
public:
	// 구 충돌
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane);

	// 박스 충돌
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere);// { return SphereAndBox(dstSphere, srcBox); }
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane);

	// 캡슐 충돌
	static bool CapapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox);
	static bool CapapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere);
	static bool CapapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane);
};

class ENGINE_DLL FLineTests
{
public:
	inline static FVector3 ClosestPointOnLineSegment(const FVector3& vA, const FVector3& vB, const FVector3& vPoint);
};

inline FVector3 FLineTests::ClosestPointOnLineSegment(const FVector3& vA, const FVector3& vB, const FVector3& vPoint)
{
	FVector3 vAB = vB - vA;
	Real fSaturate = min(max(0.f, (vPoint - vA).DotProduct(vAB) / vAB.DotProduct(vAB)), 1.f);

	return vA + vAB * fSaturate;
}

/// <summary>
/// 충돌을 관리하는 데이터
/// </summary>
struct ENGINE_DLL FCollisionData
{
public:
	FContact*	pContactArray;		// 접촉 배열
	FContact	tContacts;			// 접촉 정보

    _int		iContactsLeft;		// 충돌 남은 개수
    _uint		iContactCount;		// 충돌 개수

    Real		fFriction;			// 마찰
	Real		fRestitution;		// 반발 계수
	Real		fTolerance;			// 용인 범위


    bool HasMoreContacts()
    {
        return iContactsLeft > 0;
    }
    void Reset(unsigned maxContacts)
    {
		iContactsLeft = maxContacts;
		iContactCount = 0;
		//pContacts = pContactArray;
    }

	// 접촉 추가
    void AddContacts(unsigned count)
    {
        // Reduce the number of contacts remaining, add number used
		iContactsLeft -= count;
		iContactsLeft += count;

        // Move the array forward
		//pContacts += count;
    }
};



/// <summary>
/// 충돌에 대해 감지하는 클래스
/// IntersectTests와 다르게 충돌에 대해 이벤트를 작동시킨다.
/// </summary>
class ENGINE_DLL FCollisionDetector
{
public:
	static bool CollsionPrimitive(const FCollisionPrimitive* srcShape, const FCollisionPrimitive* dstShape, FCollisionData* pColData);
	// 구 충돌
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere, FCollisionData* pColData);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox, FCollisionData* pColData);
	static bool SphereAndCapsule(const FCollisionSphere& srcSphere, const FCollisionCapsule& dstCapsule, FCollisionData* pColData);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane, FCollisionData* pColData);
	static bool SphereAndLine(const FCollisionSphere& srcSphere, const FCollisionLine& dstLine, FCollisionData* pColData) 
	{ 
		_bool bCollide = LineAndSphere(dstLine, srcSphere, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool SphereAndRay(const FCollisionSphere& srcSphere, const FCollisionRay& dstRay, FCollisionData* pColData) 
	{ 
		_bool bCollide = RayAndSphere(dstRay, srcSphere, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool SphereAndTriangle(const FCollisionSphere& srcSphere, const FCollisionTriangle& dstTriangle, FCollisionData* pColData);
	static bool SphereAndOBB(const FCollisionSphere& srcSphere, const FCollisionOBB& dstOBB, FCollisionData* pColData);

	// 박스 충돌
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox, FCollisionData* pColData);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere, FCollisionData* pColData) 
	{ 
		_bool bCollide = SphereAndBox(dstSphere, srcBox, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool BoxAndCapsule(const FCollisionBox& srcBox, const FCollisionCapsule& dstCapsule, FCollisionData* pColData);
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane, FCollisionData* pColData);
	static bool BoxAndLine(const FCollisionBox& srcBox, const FCollisionLine& dstLine, FCollisionData* pColData) 
	{ 
		_bool bCollide = LineAndBox(dstLine, srcBox, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool BoxAndRay(const FCollisionBox& srcBox, const FCollisionRay& dstRay, FCollisionData* pColData) 
	{ 
		_bool bCollide = RayAndBox(dstRay, srcBox, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool BoxAndTriangle(const FCollisionBox& srcBox, const FCollisionTriangle& dstTriangle, FCollisionData* pColData);
	static bool BoxAndOBB(const FCollisionBox& srcBox, const FCollisionOBB& dstOBB, FCollisionData* pColData);

	// 캡슐 충돌
	static bool CapsuleAndCapsule(const FCollisionCapsule& srcCapsule, const FCollisionCapsule& dstCapsule, FCollisionData* pColData);
	static bool CapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox, FCollisionData* pColData) 
	{ 
		_bool bCollide = BoxAndCapsule(dstBox, srcCapsule, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool CapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere, FCollisionData* pColData) 
	{ 
		_bool bCollide = SphereAndCapsule(dstSphere, srcCapsule, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool CapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane, FCollisionData* pColData);
	static bool CapsuleAndLine(const FCollisionCapsule& srcCapsule, const FCollisionLine& dstLine, FCollisionData* pColData) 
	{ 
		_bool bCollide = LineAndCapsule(dstLine, srcCapsule, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool CapsuleAndRay(const FCollisionCapsule& srcCapsule, const FCollisionRay& dstRay, FCollisionData* pColData) 
	{ 
		_bool bCollide = RayAndCapsule(dstRay, srcCapsule, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool CapsuleAndTriangle(const FCollisionCapsule& srcCapsule, const FCollisionTriangle& dstTriangle, FCollisionData* pColData);
	static bool CapsuleAndOBB(const FCollisionCapsule& srcCapsule, const FCollisionOBB& dstOBB, FCollisionData* pColData);

	// 평면 충돌
	static bool PlaneAndCapsule(const FCollisionPlane& srcPlane, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
	{
		_bool bCollide = CapsuleAndPlane(dstCapsule, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool PlaneAndBox(const FCollisionPlane& srcPlane, const FCollisionBox& dstBox, FCollisionData* pColData)
	{
		_bool bCollide = BoxAndPlane(dstBox, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool PlaneAndSphere(const FCollisionPlane& srcPlane, const FCollisionSphere& dstSphere, FCollisionData* pColData)
	{
		_bool bCollide = SphereAndPlane(dstSphere, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool PlaneAndPlane(const FCollisionPlane& srcPlane, const FCollisionPlane& dstPlane, FCollisionData* pColData);// 솔직히 필없을듯, 평행인지만 검사될듯
	static bool PlaneAndLine(const FCollisionPlane& srcPlane, const FCollisionLine& dstLine, FCollisionData* pColData)
	{
		_bool bCollide = LineAndPlane(dstLine, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool PlaneAndRay(const FCollisionPlane& srcPlane, const FCollisionRay& dstRay, FCollisionData* pColData)
	{
		_bool bCollide = RayAndPlane(dstRay, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool PlaneAndTriangle(const FCollisionPlane& srcPlane, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
	{
		_bool bCollide = TriangleAndPlane(dstTriangle, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool PlaneAndOBB(const FCollisionPlane& srcPlane, const FCollisionOBB& dstOBB, FCollisionData* pColData)
	{
		_bool bCollide = OBBAndPlane(dstOBB, srcPlane, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}

	// 라인 충돌
	static bool LineAndCapsule(const FCollisionLine& srcLine, const FCollisionCapsule& dstCapsule, FCollisionData* pColData);
	static bool LineAndBox(const FCollisionLine& srcLine, const FCollisionBox& dstBox, FCollisionData* pColData);
	static bool LineAndSphere(const FCollisionLine& srcLine, const FCollisionSphere& dstSphere, FCollisionData* pColData);
	static bool LineAndPlane(const FCollisionLine& srcLine, const FCollisionPlane& dstPlane, FCollisionData* pColData);
	static bool LineAndLine(const FCollisionLine& srcLine, const FCollisionLine& dstLine, FCollisionData* pColData); // X
	static bool LineAndRay(const FCollisionLine& srcLine, const FCollisionRay& dstRay, FCollisionData* pColData);// X
	static bool LineAndTriangle(const FCollisionLine& srcLine, const FCollisionTriangle& dstTriangle, FCollisionData* pColData);
	static bool LineAndOBB(const FCollisionLine& srcLine, const FCollisionOBB& dstOBB, FCollisionData* pColData);

	// 레이 충돌
	static bool RayAndCapsule(const FCollisionRay& srcRay, const FCollisionCapsule& dstCapsule, FCollisionData* pColData);
	static bool RayAndBox(const FCollisionRay& srcRay, const FCollisionBox& dstBox, FCollisionData* pColData);
	static bool RayAndSphere(const FCollisionRay& srcRay, const FCollisionSphere& dstSphere, FCollisionData* pColData);
	static bool RayAndPlane(const FCollisionRay& srcRay, const FCollisionPlane& dstPlane, FCollisionData* pColData);
	static bool RayAndLine(const FCollisionRay& srcRay, const FCollisionLine& dstLine, FCollisionData* pColData);// X
	static bool RayAndRay(const FCollisionRay& srcRay, const FCollisionRay& dstRay, FCollisionData* pColData);// X
	static bool RayAndTriangle(const FCollisionRay& srcRay, const FCollisionTriangle& dstTriangle, FCollisionData* pColData);
	static bool RayAndOBB(const FCollisionRay& srcRay, const FCollisionOBB& dstOBB, FCollisionData* pColData);

	// 삼각형 충돌
	static bool TriangleAndCapsule(const FCollisionTriangle& srcTriangle, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
	{
		_bool bCollide = CapsuleAndTriangle(dstCapsule, srcTriangle, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool TriangleAndBox(const FCollisionTriangle& srcTriangle, const FCollisionBox& dstBox, FCollisionData* pColData)
	{
		_bool bCollide = BoxAndTriangle(dstBox, srcTriangle, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool TriangleAndSphere(const FCollisionTriangle& srcTriangle, const FCollisionSphere& dstSphere, FCollisionData* pColData)
	{
		_bool bCollide = SphereAndTriangle(dstSphere, srcTriangle, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool TriangleAndPlane(const FCollisionTriangle& srcTriangle, const FCollisionPlane& dstPlane, FCollisionData* pColData);
	static bool TriangleAndLine(const FCollisionTriangle& srcTriangle, const FCollisionLine& dstLine, FCollisionData* pColData)
	{
		_bool bCollide = LineAndTriangle(dstLine, srcTriangle, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool TriangleAndRay(const FCollisionTriangle& srcTriangle, const FCollisionRay& dstRay, FCollisionData* pColData)
	{
		_bool bCollide = RayAndTriangle(dstRay, srcTriangle, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool TriangleAndTriangle(const FCollisionTriangle& srcTriangle, const FCollisionTriangle& dstTriangle, FCollisionData* pColData);
	static bool TriangleAndOBB(const FCollisionTriangle& srcTriangle, const FCollisionOBB& dstOBB, FCollisionData* pColData);

	// OBB 충돌
	static bool OBBAndCapsule(const FCollisionOBB& srcOBB, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
	{
		_bool bCollide = CapsuleAndOBB(dstCapsule, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndBox(const FCollisionOBB& srcOBB, const FCollisionBox& dstBox, FCollisionData* pColData)
	{
		_bool bCollide = BoxAndOBB(dstBox, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndSphere(const FCollisionOBB& srcOBB, const FCollisionSphere& dstSphere, FCollisionData* pColData)
	{
		_bool bCollide = SphereAndOBB(dstSphere, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndPlane(const FCollisionOBB& srcOBB, const FCollisionPlane& dstPlane, FCollisionData* pColData)
	{
		_bool bCollide = PlaneAndOBB(dstPlane, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndLine(const FCollisionOBB& srcOBB, const FCollisionLine& dstLine, FCollisionData* pColData)
	{
		_bool bCollide = LineAndOBB(dstLine, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndRay(const FCollisionOBB& srcOBB, const FCollisionRay& dstRay, FCollisionData* pColData)
	{
		_bool bCollide = RayAndOBB(dstRay, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndTriangle(const FCollisionOBB& srcOBB, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
	{
		_bool bCollide = TriangleAndOBB(dstTriangle, srcOBB, pColData);
		pColData->tContacts.Reverse_BodyData();
		return bCollide;
	}
	static bool OBBAndOBB(const FCollisionOBB& srcOBB, const FCollisionOBB& dstOBB, FCollisionData* pColData);
};


END