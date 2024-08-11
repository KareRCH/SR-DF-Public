#include "stdafx.h"
#include "PlayerLighter.h"

CPlayerLighter::CPlayerLighter(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CPlayerLighter::CPlayerLighter(const CPlayerLighter& rhs)
	: Base(rhs)
{
}

CPlayerLighter::~CPlayerLighter()
{
}

CPlayerLighter* CPlayerLighter::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

HRESULT CPlayerLighter::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_bLightOn = false;

	return S_OK;
}

_int CPlayerLighter::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);


    Height_On_Terrain();

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    Update_InternalData();

    return S_OK;
}

void CPlayerLighter::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayerLighter::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
 
    if (m_bLightOn)
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
        m_pGraphicDev->LightEnable(2, TRUE); // ���� ���� 
        Create_Light();
    }
    else
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pGraphicDev->LightEnable(2, FALSE); // ����Ʈ �ε��� 2�� ��Ȱ��ȭ
    }
    
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // �޸�����
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bLightOn)
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CPlayerLighter::Height_On_Terrain()
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

HRESULT CPlayerLighter::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    
    return S_OK;
}

void CPlayerLighter::Update_InternalData()
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
    CBlackBoard_Player* m_pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // ���⼭���� �������� ������ ���´�.
    m_bLightOn = m_pBlackBoard->Get_LighterLight();
}

void CPlayerLighter::Free()
{
    SUPER::Free();
}

HRESULT CPlayerLighter::Create_Light()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    ZeroMemory(&AceLight, sizeof(D3DLIGHT9)); // �ʱ�ȭ 

    // �� �������ֱ� 
    AceLight.Type = D3DLIGHT_POINT; // ���������� ����
    AceLight.Position = m_pPlayerTransformcomp->Get_Pos();
    AceLight.Range = 8.f; // ������ �޴� ��ü�� ���� �� �Ÿ� 
    AceLight.Diffuse = D3DXCOLOR(DXCOLOR_ORANGE); // ���� ���� ���� - ������ �ݻ�� �� ��µǴ� ���� �ֵ� ��
    AceLight.Ambient = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // ���� �ֺ� ȯ��� ���� - ������ ��ġ�� �����ϰ� �Ȱ��� ������ ��� ������ �ݻ�Ǵ� ��
    AceLight.Falloff = 1.f;
    // POINT ������ ������ ������ ����(DIRECTION) ���� 

    // ���� - �Ÿ��� ���� ���� ���谪
    AceLight.Attenuation0 = 0.1f; // ���� ���� ��� 
    AceLight.Attenuation1 = 0.1f; // ���� ���� ��� -> ���⼺ �����ΰ��
    AceLight.Attenuation2 = 0.0f; // 2�� ���� ��� 
   

    /* LIGHT_SPOT �ϰ��, 
    	m_tLight.Type = D3DLIGHT_POINT;
    	m_tLight.Diffuse = D D3DXCOLOR(DXCOLOR_DARK_ORANGE);
		m_tLight.Ambient = D D3DXCOLOR(DXCOLOR_DARK_ORANGE);
		m_tLight.Attenuation0 = 0.15f;
		m_tLight.Attenuation1 = 0.15f;
		m_tLight.Attenuation2 = 0.f;
		m_tLight.Falloff = 1.f;
		m_tLight.Theta = D3DXToRadian(40.f);
		m_tLight.Phi = D3DXToRadian(80.f);
		m_tLight.Range = 100.f;
		m_tLight.Position = _vec3(0.f, 5.f, 0.f);
		m_tLight.Direction = _vec3(1.f, -1.5f, 0.f);*/


    // ���� ���
    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &AceLight, 2), E_FAIL);

    //���� ���� - ���� Terrain���� ������ Materialdl�� �����̶� ���⼭ �����ص� �ȵ� 
    //D3DMATERIAL9			SpotMtrl;
    //ZeroMemory(&SpotMtrl, sizeof(D3DMATERIAL9));

    //SpotMtrl.Diffuse  =  D3DXCOLOR(DXCOLOR_MAGENTA); //���ݻ� - ��ü�� ���� ���� ��������
    //SpotMtrl.Specular =  D3DXCOLOR(DXCOLOR_WHITE); //V���ݻ� - ���̶���Ʈ ǥ��
    //SpotMtrl.Ambient  =  D3DXCOLOR(DXCOLOR_RED);
    //SpotMtrl.Emissive = { 0.75f, 0.f, 0.f, 0.f }; //��ü �߱� - �׿�ȿ�� 
    //SpotMtrl.Power = 10.f;

    //m_pGraphicDev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
    //m_pGraphicDev->SetMaterial(&SpotMtrl);

    return S_OK;
}

