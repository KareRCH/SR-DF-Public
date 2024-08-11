#pragma once

#include "SceneComponent.h"

#include "PhysicsWorld3D.h"
#include "Contact.h"

BEGIN(Engine)

class FCollisionPrimitive;

/// <summary>
/// 콜라이더 컴포넌트는 충돌체 정보와 트랜스폼 정보를 저장하는 클래스입니다.
/// </summary>
class ENGINE_DLL CColliderComponent : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CColliderComponent)

protected:
	explicit CColliderComponent();
	explicit CColliderComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColliderComponent(const CColliderComponent& rhs);
	virtual ~CColliderComponent();

public:
	static	CComponent*			Create(LPDIRECT3DDEVICE9 pGraphicDev, ECOLLISION eType);
	virtual CComponent*			Clone();
	virtual void				Free();

public:
	PRIVATE virtual HRESULT Ready_Component() { return S_OK; }
	PUBLIC	virtual HRESULT Ready_Component(LPDIRECT3DDEVICE9 pGraphicDev, ECOLLISION eType);
	virtual _int	Update_Component(const _float& fTimeDelta);
	virtual void	LateUpdate_Component() {}
	virtual void	Render_Component() {}

public:
	virtual void	EnterToPhysics(_uint iIndex);
	virtual void	ExitFromPhysics(_uint iIndex);
	// 일반 업데이트가 끝난 다음에 불러오는 물리 업데이트 함수
	virtual void	Update_Physics(const _matrix& matWorld);

public:		// 충돌체 저장용 포인터
	GETSET_EX2(FCollisionPrimitive*, m_pCollisionShape, Shape, GET, SET)
	
protected:
	FCollisionPrimitive* m_pCollisionShape = nullptr;			// 충돌체 저장 포인터

public:			// 이벤트 함수
	template <typename Class>
	HRESULT	Set_Collision_Event(Class* pOwner, function<void(Class*, CGameObject*, const FContact* const)> fn);

	template <typename Class>
	HRESULT	Set_CollisionEntered_Event(Class* pOwner, function<void(Class*, CGameObject*, const FContact* const)> fn);

	template <typename Class>
	HRESULT	Set_CollisionExited_Event(Class* pOwner, function<void(Class*, CGameObject*)> fn);

protected:
	function<void(CGameObject*, const FContact* const)>		m_fnCollision;
	function<void(CGameObject*, const FContact* const)>		m_fnCollisionEntered;
	function<void(CGameObject*)>							m_fnCollisionExited;

protected:
	// 충돌이 발생할 때 불러오는 함수. 충돌이 발생하면 연결된 함수로 다시 신호를 보내줍니다.
	// 충돌중
	virtual void OnCollision(CColliderComponent* pDst, const FContact* const pContact);
	// 충돌 진입, Collide와 함께 발동
	virtual void OnCollisionEntered(CColliderComponent* pDst, const FContact* const pContact);
	// 충돌 끝남, 모든 충돌 체크가 끝나는 시점에 발동
	virtual void OnCollisionExited();

protected:
	using pair_collider = pair<CColliderComponent*, _bool>;
	// 충돌 발생시 충돌한 객체를 가리키기 위한 리스트, Collide를 통해 매 프레임 조절됩니다.
	list<pair_collider> m_listColliderObject;

public:
	GETSET_EX1(_ulong, m_dwCollisionLayer_Flag, CollisionLayer, GET_C_REF)
	void Set_CollisionLayer(const _ulong dwLayer_Flag)
	{
		m_dwCollisionLayer_Flag = dwLayer_Flag;
		if (m_pCollisionShape)
			m_pCollisionShape->Set_CollisionLayer(m_dwCollisionLayer_Flag);
	}
	GETSET_EX1(_ulong, m_dwCollisionMask_Flag, CollisionMask, GET_C_REF)
	void Set_CollisionMask(const _ulong dwMask_Flag)
	{
		m_dwCollisionMask_Flag = dwMask_Flag;
		if (m_pCollisionShape)
			m_pCollisionShape->Set_CollisionMask(m_dwCollisionMask_Flag);
	}

protected:
	_ulong				m_dwCollisionLayer_Flag;			// 콜리전 레이어, 충돌체가 존재하는 층
	_ulong				m_dwCollisionMask_Flag;				// 콜리전 마스크, 충돌체가 충돌하고 싶어하는 층

public:
	GETSET_EX1(LPD3DXMESH, m_Mesh, Mesh, GET_REF_C)

public:
	LPD3DXMESH	m_Mesh = NULL;
	void SeeColliderFrame(LPDIRECT3DDEVICE9 pGraphicDev);
};

END


template <typename Class>
inline HRESULT	CColliderComponent::Set_Collision_Event(Class* pOwner, function<void(Class*, CGameObject*, const FContact* const)> fn)
{
	m_fnCollision = [pOwner, fn](CGameObject* pDst, const FContact* const pContact) { fn(pOwner, pDst, pContact); };

	return S_OK;
}

template <typename Class>
inline HRESULT CColliderComponent::Set_CollisionEntered_Event(Class* pOwner, function<void(Class*, CGameObject*, const FContact* const)> fn)
{
	m_fnCollisionEntered = [pOwner, fn](CGameObject* pDst, const FContact* const pContact) { fn(pOwner, pDst, pContact); };

	return S_OK;
}

template <typename Class>
inline HRESULT CColliderComponent::Set_CollisionExited_Event(Class* pOwner, function<void(Class*, CGameObject*)> fn)
{
	m_fnCollisionExited = [pOwner, fn](CGameObject* pDst) { fn(pOwner, pDst); };

	return S_OK;
}
