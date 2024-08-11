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
        m_pGraphicDev->LightEnable(2, TRUE); // 누수 잡음 
        Create_Light();
    }
    else
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pGraphicDev->LightEnable(2, FALSE); // 라이트 인덱스 2를 비활성화
    }
    
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // 뒷면제거
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
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // 연결 실패
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Player* m_pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 얻어온다.
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

    ZeroMemory(&AceLight, sizeof(D3DLIGHT9)); // 초기화 

    // 값 셋팅해주기 
    AceLight.Type = D3DLIGHT_POINT; // 점광원으로 설정
    AceLight.Position = m_pPlayerTransformcomp->Get_Pos();
    AceLight.Range = 8.f; // 조명을 받는 객체의 가장 먼 거리 
    AceLight.Diffuse = D3DXCOLOR(DXCOLOR_ORANGE); // 광원 색상 설정 - 광원에 반사될 때 출력되는 가장 주된 색
    AceLight.Ambient = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // 광원 주변 환경색 설정 - 광원의 위치와 무관하게 똑같은 양으로 모든 점에서 반사되는 색
    AceLight.Falloff = 1.f;
    // POINT 점광원 에서는 광선의 방향(DIRECTION) 무시 

    // 감쇠 - 거리에 따른 선형 감쇠값
    AceLight.Attenuation0 = 0.1f; // 정적 감쇠 계수 
    AceLight.Attenuation1 = 0.1f; // 선형 감쇠 계수 -> 방향성 광원인경우
    AceLight.Attenuation2 = 0.0f; // 2차 감쇠 계수 
   

    /* LIGHT_SPOT 일경우, 
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


    // 광원 등록
    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &AceLight, 2), E_FAIL);

    //재질 설정 - 현재 Terrain에서 생성한 Materialdl이 메인이라 여기서 설정해도 안들어감 
    //D3DMATERIAL9			SpotMtrl;
    //ZeroMemory(&SpotMtrl, sizeof(D3DMATERIAL9));

    //SpotMtrl.Diffuse  =  D3DXCOLOR(DXCOLOR_MAGENTA); //난반사 - 물체의 색이 가장 밝은색임
    //SpotMtrl.Specular =  D3DXCOLOR(DXCOLOR_WHITE); //V정반사 - 하이라이트 표현
    //SpotMtrl.Ambient  =  D3DXCOLOR(DXCOLOR_RED);
    //SpotMtrl.Emissive = { 0.75f, 0.f, 0.f, 0.f }; //자체 발광 - 네온효과 
    //SpotMtrl.Power = 10.f;

    //m_pGraphicDev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
    //m_pGraphicDev->SetMaterial(&SpotMtrl);

    return S_OK;
}

