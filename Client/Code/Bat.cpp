#include "stdafx.h"
#include "Bat.h"

#include "Player.h"

CBat::CBat(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CBat::CBat(const CBat& rhs)
    : Base(rhs)
{
}

CBat::~CBat()
{
}

CBat* CBat::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 vPos, const _vec3 vRot, const _vec3 vScale)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(vPos, vRot, vScale)))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    
    return pInstance;
}

void CBat::Free()
{
    SUPER::Free();
}

HRESULT CBat::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CBat::Ready_GameObject(const _vec3 vPos, const _vec3 vRot, const _vec3 vScale)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(vRot));
    m_pTransformComp->Set_Scale(vScale);

    m_pTransformComp->Readjust_Transform();

    m_pTextureComp->Receive_Texture(TEX_CUBE, L"Furniture", L"WhiteCurtainTable");

    return S_OK;
}

_int CBat::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    _vec3 vLook = m_pTransformComp->Get_Look();
    D3DXVec3Normalize(&vLook, &vLook);
    m_pTransformComp->Set_Pos(m_pTransformComp->Get_Pos() + vLook * 10.f * fTimeDelta);

    m_pTransformComp->Readjust_Transform();

    // 물리바디 업데이트
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CBat::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBat::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pCubeBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBat::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
    // ㅋ
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pDst);
    if (pPlayer)
    {
        _vec3 vLook = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vLook, &vLook);
        pPlayer->Get_TransformComponent()->Set_Pos(
            pPlayer->Get_TransformComponent()->Get_Pos() + vLook * 1.f);

        Engine::Play_Sound(L"FallenAces", L"Punch (0).wav", SOUND_ENEMY_NORMAL_EFFECT, 0.9f);
    }
}

void CBat::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CBat::OnCollisionExited(CGameObject* pDst)
{
}

HRESULT CBat::Add_Component()
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
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER);

    return S_OK;
}
