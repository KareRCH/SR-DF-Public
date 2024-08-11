#include "stdafx.h"
#include "PlayerGunLighter.h"

CPlayerGunLighter::CPlayerGunLighter(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CPlayerGunLighter::CPlayerGunLighter(const CPlayerGunLighter& rhs)
    : Base(rhs)
{
}

CPlayerGunLighter::~CPlayerGunLighter()
{
}

CPlayerGunLighter* CPlayerGunLighter::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Lighter Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CPlayerGunLighter::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_bLightOn = FALSE;

    return S_OK;
}

_int CPlayerGunLighter::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Update_BlackBoard();

    if(m_bLightOn)
        Create_Light();

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CPlayerGunLighter::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayerGunLighter::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    if (m_bLightOn)
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
        m_pGraphicDev->LightEnable(4, TRUE); // ���� ���� 
    }
    else
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pGraphicDev->LightEnable(4, FALSE); // ����Ʈ �ε��� 2�� ��Ȱ��ȭ
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // �޸�����
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bLightOn)
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}


HRESULT CPlayerGunLighter::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    return S_OK;
}

void CPlayerGunLighter::Free()
{
    SUPER::Free();
}

void CPlayerGunLighter::Update_BlackBoard()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // ���� ����
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // ���⼭���� �������� ������ ������Ʈ �Ѵ�.
    m_bLightOn = pBlackBoard->Get_GunLight();
}

HRESULT CPlayerGunLighter::Create_Light()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    ZeroMemory(&AceLight, sizeof(D3DLIGHT9)); // �ʱ�ȭ 

    // �� �������ֱ� 
    AceLight.Type = D3DLIGHT_POINT; // ���������� ����
    AceLight.Position = m_pPlayerTransformcomp->Get_Pos();
    AceLight.Range = 4.f; // ������ �޴� ��ü�� ���� �� �Ÿ� 
    AceLight.Diffuse = D3DXCOLOR(DXCOLOR_WHITE); // ���� ���� ���� - ������ �ݻ�� �� ��µǴ� ���� �ֵ� ��
    AceLight.Ambient = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // ���� �ֺ� ȯ��� ���� - ������ ��ġ�� �����ϰ� �Ȱ��� ������ ��� ������ �ݻ�Ǵ� ��
    AceLight.Falloff = 1.f;
    // POINT ������ ������ ������ ����(DIRECTION) ���� 

    // ���� - �Ÿ��� ���� ���� ���谪
    AceLight.Attenuation0 = 0.1f; // ���� ���� ��� 
    AceLight.Attenuation1 = 0.1f; // ���� ���� ��� -> ���⼺ �����ΰ��
    AceLight.Attenuation2 = 0.0f; // 2�� ���� ��� 

    // ���� ���
    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &AceLight, 4), E_FAIL);

    return S_OK;
}

