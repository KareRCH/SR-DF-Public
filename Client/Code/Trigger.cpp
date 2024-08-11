#include "stdafx.h"
#include "Trigger.h"

#include "Export_System.h"
#include "Export_Utility.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : Base(rhs)
{
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CTrigger::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return _int();
}

void CTrigger::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTrigger::Free()
{
    SUPER::Free();
}

void CTrigger::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CTrigger::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

HRESULT CTrigger::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderOBBComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_TRIGGER);
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER);

    return S_OK;
}
