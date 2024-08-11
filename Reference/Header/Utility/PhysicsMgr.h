#pragma once

#include "Base.h"
#include "Engine_Define.h"

#include "PhysicsWorld3D.h"
#include "ColliderComponent.h"


BEGIN(Engine)

/// <summary>
/// 물리 세계를 관리하는 클래스
/// </summary>
class ENGINE_DLL CPhysicsMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CPhysicsMgr)

private:
	explicit CPhysicsMgr();
	virtual ~CPhysicsMgr();

public:
	virtual void	Free();

public:
	HRESULT			Ready_Physics(const _uint iMaxPhysicsWorld3D);
	void			StartFrame_Physics();
	// 물리 정확도를 위해 Precision의 실수 타입을 사용한다.
	_int			Update_Physics(const Real& fTimeDelta);

public:
	GETSET_EX1(vector<CPhysicsWorld3D*>, m_vecWorld3D, VecWorld3D, GET_PTR)
	CPhysicsWorld3D* Get_World3D(const _uint iWorldID) const { return m_vecWorld3D[iWorldID]; }

private:
	vector<CPhysicsWorld3D*> m_vecWorld3D;

public:	// 물리세계에 충돌 테스트를 진행후 그 리스트를 돌려받기
	list<pair<CGameObject*, FContact>> IntersectTests_GetGameObject(const _uint iWorldID, FCollisionPrimitive* pSrc);

	list<pair<CGameObject*, FContact>> IntersectTests_Collider_GetGameObject(const _uint iWorldID, const _vec3 vPos, CColliderComponent* pSrc, _ulong iMask);
	list<pair<CGameObject*, FContact>> IntersectTests_Sphere_GetGameObject(const _uint iWorldID, _vec3 vPos, _float fRadius, _ulong iMask);
	list<pair<CGameObject*, FContact>> IntersectTests_Box_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize, _ulong iMask);
	list<pair<CGameObject*, FContact>> IntersectTests_Capsule_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius, _ulong iMask);
	list<pair<CGameObject*, FContact>> IntersectTests_Line_GetGameObject(const _uint iWorldID, _vec3 vStart, _vec3 vEnd, _ulong iMask);
	list<pair<CGameObject*, FContact>> IntersectTests_Ray_GetGameObject(const _uint iWorldID, const _vec3 vPos, const _vec3 vNormal, _ulong iMask);
};

END