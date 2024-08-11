#pragma once

#include "SceneComponent.h"

#include "PhysicsWorld3D.h"
#include "Contact.h"

BEGIN(Engine)

class FCollisionPrimitive;

/// <summary>
/// �ݶ��̴� ������Ʈ�� �浹ü ������ Ʈ������ ������ �����ϴ� Ŭ�����Դϴ�.
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
	// �Ϲ� ������Ʈ�� ���� ������ �ҷ����� ���� ������Ʈ �Լ�
	virtual void	Update_Physics(const _matrix& matWorld);

public:		// �浹ü ����� ������
	GETSET_EX2(FCollisionPrimitive*, m_pCollisionShape, Shape, GET, SET)
	
protected:
	FCollisionPrimitive* m_pCollisionShape = nullptr;			// �浹ü ���� ������

public:			// �̺�Ʈ �Լ�
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
	// �浹�� �߻��� �� �ҷ����� �Լ�. �浹�� �߻��ϸ� ����� �Լ��� �ٽ� ��ȣ�� �����ݴϴ�.
	// �浹��
	virtual void OnCollision(CColliderComponent* pDst, const FContact* const pContact);
	// �浹 ����, Collide�� �Բ� �ߵ�
	virtual void OnCollisionEntered(CColliderComponent* pDst, const FContact* const pContact);
	// �浹 ����, ��� �浹 üũ�� ������ ������ �ߵ�
	virtual void OnCollisionExited();

protected:
	using pair_collider = pair<CColliderComponent*, _bool>;
	// �浹 �߻��� �浹�� ��ü�� ����Ű�� ���� ����Ʈ, Collide�� ���� �� ������ �����˴ϴ�.
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
	_ulong				m_dwCollisionLayer_Flag;			// �ݸ��� ���̾�, �浹ü�� �����ϴ� ��
	_ulong				m_dwCollisionMask_Flag;				// �ݸ��� ����ũ, �浹ü�� �浹�ϰ� �;��ϴ� ��

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
