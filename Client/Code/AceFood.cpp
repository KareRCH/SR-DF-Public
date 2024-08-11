#include "stdafx.h"
#include "AceFood.h"
#include "UI_EatFood.h"
#include "Effect_Bubble.h"
#include <AceBuilding.h>

CAceFood::CAceFood(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceFood::CAceFood(const CAceFood& rhs)
    : Base(rhs)
{
}

CAceFood::~CAceFood()
{
}

CAceFood* CAceFood::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (!pInstance)
        return nullptr;

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }
    
    pInstance->m_pTransformComp->Set_Pos({ _fx, _fy, _fz });
    pInstance->FoodName(pObjTag);
    pInstance->CurPos = { _fx, _fy, _fz };

    return pInstance;
}

CAceFood* CAceFood::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (!pInstance)
        return nullptr;

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

// Collision - 트리거 발동용 (event방식)
void CAceFood::OnCollision(CGameObject* pDst, const FContact* const pContact) // 계속 충돌중 
{
    CAceBuilding* pSolid = dynamic_cast<CAceBuilding*>(pDst);
    if (pSolid)
    {
        _vec3 vNormal(_float(pContact->vContactNormal.x), _float(pContact->vContactNormal.y), _float(pContact->vContactNormal.z));
        m_pTransformComp->Set_Pos((m_pTransformComp->Get_Pos() - vNormal * static_cast<_float>(pContact->fPenetration)));
        if (D3DXVec3Dot(&(-vNormal), &_vec3({ 0.f, -1.f, 0.f })) < 0.f)
            m_IsOnGround = true;
    }
}

void CAceFood::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) // 처음 충동 진입 
{
}

void CAceFood::OnCollisionExited(CGameObject* pDst) // 충돌 나갈때 
{
}

HRESULT CAceFood::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CAceFood::Ready_GameObject(const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    CurPos = m_pTransformComp->Get_Pos();

    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return S_OK;
}

HRESULT CAceFood::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    if (!tObjectSerial.strGroupKey.empty() && !tObjectSerial.strTextureKey.empty())
        m_pTextureComp->Receive_Texture(TEX_CUBE,
            wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str()
            , wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());

    m_eFactoryClass = OBJECT_CLASS::FOOD;

    Serialize_FoodName(strConvName);

    return S_OK;
}
                               
_int CAceFood::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Gravity(fTimeDelta);

    m_pTransformComp->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

    // 지형타기 
    if (m_pTransformComp->Get_Pos().y < 1.5f && m_vSpeed.y < 0.f)
    {
        Height_On_Terrain();
        m_IsOnGround = true;
    }
    else
        m_IsOnGround = false;


    // 빌보드 
    BillBoard(fTimeDelta);
    
    // 변수에 저장된 enum으로 texture 결정 - eaten 변경때문에 
    Eat_Food(m_pCurName, fTimeDelta, m_bEaten);

    if (m_bDead)
        Set_Dead();

    // 물리바디 업데이트
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceFood::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceFood::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceFood::Free()
{
    SUPER::Free();
}

HRESULT CAceFood::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    //NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderOBBComp"), E_FAIL);

    // 충돌 컴포넌트 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_ITEM); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_WALL); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

    m_pColliderComp->Set_Scale({ 1.22f, 1.22f, 1.22f });

    return S_OK;
}

void CAceFood::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
    if (nullptr == pTerrainBufferComp)
        return;

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight +0.25f, vPos.z);
}

HRESULT CAceFood::BillBoard(const _float& fTimeDelta)
{
    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);

    return S_OK;
}

void CAceFood::FoodName(const _tchar* pObjTag)
{
    if ((wcscmp(pObjTag, L"APPLE") == 0) || (wcscmp(pObjTag, L"Apple") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::APPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Apple");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
        m_fHp = 5.f;
    }
    else if ((wcscmp(pObjTag, L"EATENAPPLE") == 0) || (wcscmp(pObjTag, L"EatenApple") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::EATENAPPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"EatenApple");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
        m_fHp = 0.f;
    }
    else if ((wcscmp(pObjTag, L"BANANA") == 0) || (wcscmp(pObjTag, L"Banana") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::BANANA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Banana");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
        m_fHp = 5.f;
    }
    else if ((wcscmp(pObjTag, L"BANANAPEEL") == 0) || (wcscmp(pObjTag, L"BananaPeel") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::BANANAPEEL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"BananaPeel");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
        m_fHp = 0.f;
    }
    else if ((wcscmp(pObjTag, L"COLA") == 0) || (wcscmp(pObjTag, L"Cola") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::COLA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Cola");
        m_pTransformComp->Set_Scale({ 0.1f, 1.f,  0.5f });
        m_fHp = 12.f;
    }
    else if ((wcscmp(pObjTag, L"MEDIKIT") == 0) || (wcscmp(pObjTag, L"Medkit") == 0))
    {
        m_pCurName = CAceFood::FOOD_NAME::MEDIKIT;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Medkit");
        m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });
        m_fHp = 20.f;
    }
    else
    {
        m_pCurName = CAceFood::FOOD_NAME::FOOD_END;
        m_fHp = 0.f;
    }
}

void CAceFood::Eat_Food(FOOD_NAME eCurName, const _float& fTimeDelta, _bool bEat)
{
    if (bEat)
    {
        switch (eCurName)
        {
        case CAceFood::FOOD_NAME::APPLE:
            Engine::Add_GameObject(L"UI", CUI_EatFood::Create(m_pGraphicDev));
            Engine::Play_Sound(L"Food", L"ummm.wav", SOUND_PLAYER, 1.f);
            Engine::Play_Sound(L"Food", L"BiteApple.mp3", SOUND_PLAYER_EFFECT, 0.5f);
            m_pCurName = CAceFood::FOOD_NAME::EATENAPPLE;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"EatenApple");
            Engine::Add_GameObject(L"GameLogic", CEffect_Bubble::Create(m_pGraphicDev, 
                m_pTransformComp->Get_Pos().x, 
                m_pTransformComp->Get_Pos().y,
                m_pTransformComp->Get_Pos().z));
            break;

        case CAceFood::FOOD_NAME::BANANA:
            Engine::Add_GameObject(L"UI", CUI_EatFood::Create(m_pGraphicDev));
            Engine::Play_Sound(L"Food", L"ummm.wav", SOUND_PLAYER, 1.f);
            Engine::Play_Sound(L"Food", L"EatBanana.wav", SOUND_PLAYER_EFFECT, 0.5f);
            m_pCurName = CAceFood::FOOD_NAME::BANANAPEEL;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"BananaPeel");
            Engine::Add_GameObject(L"GameLogic", CEffect_Bubble::Create(m_pGraphicDev,
                                     m_pTransformComp->Get_Pos().x,
                                     m_pTransformComp->Get_Pos().y,
                                     m_pTransformComp->Get_Pos().z)); m_bDead = FALSE;
            break;

        case CAceFood::FOOD_NAME::COLA:
            Engine::Add_GameObject(L"UI", CUI_EatFood::Create(m_pGraphicDev));
             Engine::Play_Sound(L"Food", L"uhmmm.wav", SOUND_PLAYER, 1.f);
             Engine::Play_Sound(L"Food", L"DrinkSoda.wav", SOUND_PLAYER_EFFECT, 0.3f);
             Engine::Add_GameObject(L"GameLogic", CEffect_Bubble::Create(m_pGraphicDev,
                                    m_pTransformComp->Get_Pos().x,
                                    m_pTransformComp->Get_Pos().y,
                                    m_pTransformComp->Get_Pos().z));
             Set_Dead();
            break;

        case CAceFood::FOOD_NAME::MEDIKIT:
            Engine::Add_GameObject(L"UI", CUI_EatFood::Create(m_pGraphicDev));
            Engine::Play_Sound(L"Food", L"uhmmm.wav", SOUND_PLAYER, 1.f);
            Engine::Play_Sound(L"Food", L"FirstAid.wav", SOUND_PLAYER_EFFECT, 0.3f);
            Engine::Add_GameObject(L"GameLogic", CEffect_Bubble::Create(m_pGraphicDev,
                                    m_pTransformComp->Get_Pos().x,
                                    m_pTransformComp->Get_Pos().y,
                                    m_pTransformComp->Get_Pos().z));
            Set_Dead();
            break;

        case CAceFood::FOOD_NAME::EATENAPPLE:
            Engine::Play_Sound(L"Food", L"Huh.wav", SOUND_PLAYER, 0.5f);
                break;

        case CAceFood::FOOD_NAME::BANANAPEEL:
            Engine::Play_Sound(L"Food", L"Huh.wav", SOUND_PLAYER, 0.5f);
               break;
        }

        m_bEaten = false;
    }
}
void CAceFood::Serialize_FoodName(wstring _ObjName)
{
    wstring NewObj = _ObjName.substr(0, 4);

    if(NewObj.compare(L"Appl") == 0)
    {
        m_pCurName = CAceFood::FOOD_NAME::APPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Apple");
        m_fHp = 5.f;
    }
    else if (NewObj.compare(L"Peel") == 0)
    {
        m_pCurName = CAceFood::FOOD_NAME::EATENAPPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"EatenApple");
        m_fHp = 0.f;
    }
    else if (NewObj.compare(L"Bana") == 0)
    {
        m_pCurName = CAceFood::FOOD_NAME::BANANA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Banana");
        m_fHp = 5.f;
    }
    else if (NewObj.compare(L"Bana") == 0)
    {
        m_pCurName = CAceFood::FOOD_NAME::BANANAPEEL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"BananaPeel");
        m_fHp = 0.f;
    }
    else if (NewObj.compare(L"Cola") == 0)
    {
        m_pCurName = CAceFood::FOOD_NAME::COLA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Cola");
        m_fHp = 12.f;
    }
    else if (NewObj.compare(L"Medk") == 0)
    {
        m_pCurName = CAceFood::FOOD_NAME::MEDIKIT;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Medkit");
        m_fHp = 20.f;
    }
    else
    {
        m_pCurName = CAceFood::FOOD_NAME::FOOD_END;
        m_fHp = 0.f;
    }
}
/*
APPLE, BANANA, COLA, MEDIKIT,
EATENAPPLE, BANANAPEEL,
FOOD_END
*/