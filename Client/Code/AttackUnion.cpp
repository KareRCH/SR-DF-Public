#include "stdafx.h"
#include "AttackUnion.h"

CAttackUnion::CAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev)
    :Base(pGraphicDev)
{
}

CAttackUnion::CAttackUnion(const CAttackUnion& rhs)
    : Base(rhs)
{
}

CAttackUnion::~CAttackUnion()
{
}

HRESULT CAttackUnion::Ready_GameObject()
{
    return S_OK;
}

_int CAttackUnion::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CAttackUnion::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAttackUnion::Render_GameObject()
{
}

void CAttackUnion::Free()
{
    SUPER::Free();
}

void CAttackUnion::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
    SUPER::MeshBoxColider(_Width, _Height, Depth);
}

void CAttackUnion::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
    SUPER::MeshSphereColider(Radius, Slices, Stacks);
}
