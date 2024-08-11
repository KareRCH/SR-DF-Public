#include "PhysicsMgr.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

// ���� �߰� Ray�� ����ϴ� ��ũ��
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

//															������� ����,					�´� ���,		��� ����
//bool CPhysicsMgr::CollisionRayToCube(const CColliderComponent* pCRay, const CColliderComponent* pCBox, _vec3 vRayDest)
//{
//	//_vec3 vRayOrigin = pCRay->m_tCollider.vCenter;
//	_vec3 vRayOrigin = pCRay->Get_Pos(); // ������ ����(������)
//	
//	_vec3 vRayDir = vRayDest - vRayOrigin; // ������ ����
//	D3DXVec3Normalize(&vRayDir, &vRayDir); // ���� ���� ��ֶ�����
//
//	_vec3 vRayToCenter = pCBox->Get_Pos() - vRayOrigin; // ������ �������� AABB(���)�� �߽ɱ����� ����
//
//	_vec3 vAxisDotToRD = { D3DXVec3Dot(&pCBox->m_tCollider.axis[0], &vRayDir), // ������ ����� AABB(���)�� �� ����� �������� ������ ����
//						D3DXVec3Dot(&pCBox->m_tCollider.axis[1], &vRayDir),
//						D3DXVec3Dot(&pCBox->m_tCollider.axis[2], &vRayDir) };
//
//	_vec3 vAxisDotToRTC = { D3DXVec3Dot(&pCBox->m_tCollider.axis[0], &vRayToCenter), // ������ �������� AABB(���)�� �� ������� ���Ϳ��� �������� ������ ����
//					   D3DXVec3Dot(&pCBox->m_tCollider.axis[1], &vRayToCenter),
//					   D3DXVec3Dot(&pCBox->m_tCollider.axis[2], &vRayToCenter) };
//
//	float Temp[6] = {};
//
//	// �� �࿡ ���� �ݺ���
//	for (int i = 0; i < 3; ++i)
//	{
//		// ������ ������ ������ ���
//		if (CMP(vAxisDotToRD[i], 0))
//		{
//			// ������ ������ ������ ���
//			if (-vAxisDotToRTC[i] - pCBox->m_tCollider.vSize[i] > 0 || -vAxisDotToRTC.x + pCBox->m_tCollider.vSize[i] < 0)
//			{
//				return false; // ���� �ۿ� �ִ� ��� ���� ����
//			}
//			vAxisDotToRD[i] = 0.00001f; // 0���� ������ ���� �����ϱ� ���� ���� ��
//		}
//
//		// �࿡ ���� �ּ� �� �ִ밪 ���
//		Temp[i * 2 + 0] = (vAxisDotToRTC[i] + pCBox->m_tCollider.vSize[i]) / vAxisDotToRD[i]; // tmin[x, y, z]
//		Temp[i * 2 + 1] = (vAxisDotToRTC[i] - pCBox->m_tCollider.vSize[i]) / vAxisDotToRD[i]; // tmax[x, y, z]
//	}
//
//	// ������ AABB�� �����ϴ��� ���θ� Ȯ���ϱ� ���� 'tmin' �� 'tmax' �� ���
//	float tmin = fmaxf(fmaxf(fminf(Temp[0], Temp[1]), fminf(Temp[2], Temp[3])), fminf(Temp[4], Temp[5]));
//	float tmax = fminf(fminf(fmaxf(Temp[0], Temp[1]), fmaxf(Temp[2], Temp[3])), fmaxf(Temp[4], Temp[5]));
//
//
//	// 'tmax'�� 0���� ������ ������ AABB�� ���������� AABB ��ü�� ������ ���� �ڿ� ����
//	if (tmax < 0) {
//		return false;
//	}
//
//	// 'tmin'�� 'tmax'���� ũ�� ������ AABB�� �������� ����
//	if (tmin > tmax) {
//		return false;
//	}
//
//	// 'tmin'�� 0���� ������ 'tmax'�� �� ����� ���� ������ ��Ÿ��
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