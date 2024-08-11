#include "stdafx.h"
#include "BossLight.h"

CBossLight::CBossLight(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CBossLight::CBossLight(const CBossLight& rhs)
    : Base(rhs)
{
}

CBossLight::~CBossLight()
{
}

CBossLight* CBossLight::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Lighter Create Failed");
        return nullptr;
    }

    pInstance->Set_Owner(pOwner);

    return pInstance;
}

void CBossLight::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CBossLight::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CBossLight::OnCollisionExited(CGameObject* pDst)
{
}

HRESULT CBossLight::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


    m_bLightOn = FALSE;

    return S_OK;
}

_int CBossLight::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Height_On_Terrain();

    Update_BlackBoard();

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CBossLight::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBossLight::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    if (m_bLightOn)
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
        m_pGraphicDev->LightEnable(3, TRUE); // ���� ���� 
        Create_Light();
    }
    else
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pGraphicDev->LightEnable(3, FALSE); // ����Ʈ �ε��� 2�� ��Ȱ��ȭ
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // �޸�����
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bLightOn)
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CBossLight::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
    pTerrainBufferComp->Get_VtxPos(),
    pTerrainBufferComp->Get_VertexCountX() + 1U,
    pTerrainBufferComp->Get_VertexCountZ() + 1U,
    pTerrainBufferComp->Get_Scale(),
    pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

HRESULT CBossLight::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    return S_OK;
}

void CBossLight::Free()
{
    SUPER::Free();
}


HRESULT CBossLight::Create_Light()
{
    m_pBossTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Boss", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pBossTransformcomp, -1);

    ZeroMemory(&AceBossLight, sizeof(D3DLIGHT9)); // �ʱ�ȭ 

    // �� �������ֱ� 
    AceBossLight.Type = D3DLIGHT_POINT; // ���������� ����
    AceBossLight.Position = m_pBossTransformcomp->Get_Pos();
    AceBossLight.Range = 5.f; // ������ �޴� ��ü�� ���� �� �Ÿ� 
    AceBossLight.Diffuse = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // ���� ���� ���� - ������ �ݻ�� �� ��µǴ� ���� �ֵ� ��
    AceBossLight.Ambient = D3DXCOLOR(DXCOLOR_CYAN); // ���� �ֺ� ȯ��� ���� - ������ ��ġ�� �����ϰ� �Ȱ��� ������ ��� ������ �ݻ�Ǵ� ��
    AceBossLight.Falloff = 1.f;
    // BossPOINT ������ ������ ������ ����(DIRECTION) ���� 
    // 
    // Boss���� - �Ÿ��� ���� ���� ���谪
    AceBossLight.Attenuation0 = 0.6f; // ���� ���� ��� 
    AceBossLight.Attenuation1 = 0.1f; // ���� ���� ��� -> ���⼺ �����ΰ��
    AceBossLight.Attenuation2 = 0.0f; // 2�� ���� ��� 

    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &AceBossLight, 3), E_FAIL);

    return S_OK;
}

void CBossLight::Update_BlackBoard()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Monster.Get_BlackBoard())
    {
        m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
        // ���� ����
        if (!m_wpBlackBoard_Monster.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

    // ���⼭���� �������� ������ ������Ʈ �Ѵ�.
    m_bLightOn = pBlackBoard->Get_ControlLight();
}