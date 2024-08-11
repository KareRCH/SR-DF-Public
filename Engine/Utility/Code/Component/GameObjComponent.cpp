#include "GameObjComponent.h"

CGameObjComponent::CGameObjComponent()
{
}

CGameObjComponent::CGameObjComponent(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CGameObjComponent::CGameObjComponent(const CGameObjComponent& rhs)
    : Base(rhs)
{
}

CGameObjComponent::~CGameObjComponent()
{
}

void CGameObjComponent::Free()
{
    SUPER::Free();
}
