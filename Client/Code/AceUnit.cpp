#include "stdafx.h"
#include "AceUnit.h"

CAceUnit::CAceUnit(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceUnit::CAceUnit(const CAceUnit& rhs)
    : Base(rhs)
{
}

CAceUnit::~CAceUnit()
{
}

HRESULT CAceUnit::Ready_GameObject()
{
    return S_OK;
}

_int CAceUnit::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CAceUnit::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceUnit::Render_GameObject()
{
}

void CAceUnit::Free()
{
    SUPER::Free();
}

void CAceUnit::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
    SUPER::MeshBoxColider(_Width, _Height, Depth);
}

void CAceUnit::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
    SUPER::MeshSphereColider(Radius, Slices, Stacks);
}

void CAceUnit::Gravity(const _float& fTimeDelta)
{
    if (!m_IsOnGround)
        m_vSpeed += m_vAccel * fTimeDelta;
}
