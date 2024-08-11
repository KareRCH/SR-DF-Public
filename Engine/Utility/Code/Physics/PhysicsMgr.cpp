#include "PhysicsMgr.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

// 성희 추가 Ray에 사용하는 매크로
#define CMP(x, y) \
      (fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

CPhysicsMgr::CPhysicsMgr()
{

}

CPhysicsMgr::~CPhysicsMgr()
{
	Free();
}

void CPhysicsMgr::Free()
{
	for (auto item : m_vecWorld3D)
	{
		Safe_Release(item);
	}
}

HRESULT CPhysicsMgr::Ready_Physics(const _uint iMaxPhysicsWorld3D)
{
	m_vecWorld3D.reserve(iMaxPhysicsWorld3D);
	m_vecWorld3D.clear();

	for (_uint i = 0; i < m_vecWorld3D.capacity(); i++)
	{
		m_vecWorld3D.push_back(CPhysicsWorld3D::Create(1000, 1000));
	}

	return S_OK;
}

void CPhysicsMgr::StartFrame_Physics()
{
	for (auto iter = m_vecWorld3D.begin(); iter != m_vecWorld3D.end(); ++iter)
	{
		(*iter)->StartFrame_Physics();
	}
}

_int CPhysicsMgr::Update_Physics(const Real& fTimeDelta)
{
	for (auto iter = m_vecWorld3D.begin(); iter != m_vecWorld3D.end(); ++iter)
	{
		(*iter)->Update_Physics(fTimeDelta);
	}

	return 0;
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_GetGameObject(const _uint iWorldID, FCollisionPrimitive* pSrc)
{
	list_collide_test listCollision;
	list<pair<CGameObject*, FContact>> listGameObject;

	listCollision = m_vecWorld3D[iWorldID]->Test_Contacts(static_cast<FCollisionPrimitive*>(pSrc));
	for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
	{
		CColliderComponent* pCollideComp = static_cast<CColliderComponent*>((*iter).first->Get_Owner());
		CGameObject* pObj = static_cast<CGameObject*>(pCollideComp->Get_Owner());
		listGameObject.push_back(pair<CGameObject*, FContact>(pObj, (*iter).second));
	}

	return listGameObject;
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Collider_GetGameObject(const _uint iWorldID, const _vec3 vPos, CColliderComponent* pSrc, _ulong iMask)
{
	FCollisionPrimitive* pShape = static_cast<FCollisionPrimitive*>(pSrc->Get_Shape());
	pShape->Set_Position(pShape->Get_Position() + FVector3(vPos.x, vPos.y, vPos.z));
	pShape->Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, pShape);
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Sphere_GetGameObject(const _uint iWorldID, _vec3 vPos, _float fRadius, _ulong iMask)
{
	FCollisionSphere pShape;
	pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.fRadius = fRadius;
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Box_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize, _ulong iMask)
{
	FCollisionBox pShape;
	pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vHalfSize = FVector3(vHalfSize.x, vHalfSize.y, vHalfSize.z);
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Capsule_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius, _ulong iMask)
{
	FCollisionCapsule pShape;
	pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vDirHalfSize = FVector3(vNormal.x, vNormal.y, vNormal.z);
	pShape.fRadius = fRadius;
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Line_GetGameObject(const _uint iWorldID, _vec3 vStart, _vec3 vEnd, _ulong iMask)
{
	FCollisionLine pShape;
	//pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vStart = FVector3(vStart.x, vStart.y, vStart.z);
	pShape.vEnd = FVector3(vEnd.x, vEnd.y, vEnd.z);
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Ray_GetGameObject(const _uint iWorldID, const _vec3 vPos, const _vec3 vNormal, _ulong iMask)
{
	FCollisionRay pShape;
	pShape.vOrigin = (FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vDir = FVector3(vNormal.x, vNormal.y, vNormal.z).Unit();
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

//															광선쏘는 주인,					맞는 대상,		쏘는 방향
//bool CPhysicsMgr::CollisionRayToCube(const CColliderComponent* pCRay, const CColliderComponent* pCBox, _vec3 vRayDest)
//{
//	//_vec3 vRayOrigin = pCRay->m_tCollider.vCenter;
//	_vec3 vRayOrigin = pCRay->Get_Pos(); // 광선의 원점(시작점)
//	
//	_vec3 vRayDir = vRayDest - vRayOrigin; // 광선의 방향
//	D3DXVec3Normalize(&vRayDir, &vRayDir); // 방향 벡터 노멀라이즈
//
//	_vec3 vRayToCenter = pCBox->Get_Pos() - vRayOrigin; // 광선의 원점에서 AABB(대상)의 중심까지의 벡터
//
//	_vec3 vAxisDotToRD = { D3DXVec3Dot(&pCBox->m_tCollider.axis[0], &vRayDir), // 광선의 방향과 AABB(대상)의 각 축과의 내적값을 가지는 벡터
//						D3DXVec3Dot(&pCBox->m_tCollider.axis[1], &vRayDir),
//						D3DXVec3Dot(&pCBox->m_tCollider.axis[2], &vRayDir) };
//
//	_vec3 vAxisDotToRTC = { D3DXVec3Dot(&pCBox->m_tCollider.axis[0], &vRayToCenter), // 광선의 원점에서 AABB(대상)의 각 축까지의 벡터와의 내적값을 가지는 벡터
//					   D3DXVec3Dot(&pCBox->m_tCollider.axis[1], &vRayToCenter),
//					   D3DXVec3Dot(&pCBox->m_tCollider.axis[2], &vRayToCenter) };
//
//	float Temp[6] = {};
//
//	// 각 축에 대한 반복문
//	for (int i = 0; i < 3; ++i)
//	{
//		// 광선과 슬랩이 평행한 경우
//		if (CMP(vAxisDotToRD[i], 0))
//		{
//			// 광선과 슬랩이 평행한 경우
//			if (-vAxisDotToRTC[i] - pCBox->m_tCollider.vSize[i] > 0 || -vAxisDotToRTC.x + pCBox->m_tCollider.vSize[i] < 0)
//			{
//				return false; // 슬랩 밖에 있는 경우 조기 종료
//			}
//			vAxisDotToRD[i] = 0.00001f; // 0으로 나누는 것을 방지하기 위한 작은 값
//		}
//
//		// 축에 대한 최소 및 최대값 계산
//		Temp[i * 2 + 0] = (vAxisDotToRTC[i] + pCBox->m_tCollider.vSize[i]) / vAxisDotToRD[i]; // tmin[x, y, z]
//		Temp[i * 2 + 1] = (vAxisDotToRTC[i] - pCBox->m_tCollider.vSize[i]) / vAxisDotToRD[i]; // tmax[x, y, z]
//	}
//
//	// 광선이 AABB와 교차하는지 여부를 확인하기 위해 'tmin' 및 'tmax' 값 계산
//	float tmin = fmaxf(fmaxf(fminf(Temp[0], Temp[1]), fminf(Temp[2], Temp[3])), fminf(Temp[4], Temp[5]));
//	float tmax = fminf(fminf(fmaxf(Temp[0], Temp[1]), fmaxf(Temp[2], Temp[3])), fmaxf(Temp[4], Temp[5]));
//
//
//	// 'tmax'가 0보다 작으면 광선은 AABB와 교차하지만 AABB 전체가 광선의 원점 뒤에 있음
//	if (tmax < 0) {
//		return false;
//	}
//
//	// 'tmin'이 'tmax'보다 크면 광선은 AABB와 교차하지 않음
//	if (tmin > tmax) {
//		return false;
//	}
//
//	// 'tmin'이 0보다 작으면 'tmax'가 더 가까운 교차 지점을 나타냄
//	float t_result = tmin;
//	if (tmin < 0.0f) {
//		t_result = tmax;
//	}
//
//	_float fDist = D3DXVec3Length(&(vRayDir * t_result));
//	if (fDist < D3DXVec3Length(&(vRayDest - vRayOrigin)))
//		return true;
//	else
//		return false;
//}