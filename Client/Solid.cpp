#include "stdafx.h"
#include "Solid.h"

#include "Export_System.h"
#include "Export_Utility.h"

CSolid::CSolid(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CSolid::CSolid(const CSolid& rhs)
	: Base(rhs)
{
}

CSolid::~CSolid()
{
}

void CSolid::Free()
{
    SUPER::Free();
}

HRESULT CSolid::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	return S_OK;
}

_int CSolid::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	return 0;
}

void CSolid::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CSolid::Render_GameObject()
{
}

void CSolid::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CSolid::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CSolid::OnCollisionExited(CGameObject* pDst)
{
}

HRESULT CSolid::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_CubeBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderOBBComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_WALL);
    //m_pColliderComp->Set_CollisionMask(LAYER_PLAYER);

    return S_OK;
}
