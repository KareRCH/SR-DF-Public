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
        m_pGraphicDev->LightEnable(4, TRUE); // 누수 잡음 
    }
    else
    {
        m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
        m_pGraphicDev->LightEnable(4, FALSE); // 라이트 인덱스 2를 비활성화
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // 뒷면제거
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
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // 연결 실패
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 업데이트 한다.
    m_bLightOn = pBlackBoard->Get_GunLight();
}

HRESULT CPlayerGunLighter::Create_Light()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    ZeroMemory(&AceLight, sizeof(D3DLIGHT9)); // 초기화 

    // 값 셋팅해주기 
    AceLight.Type = D3DLIGHT_POINT; // 점광원으로 설정
    AceLight.Position = m_pPlayerTransformcomp->Get_Pos();
    AceLight.Range = 4.f; // 조명을 받는 객체의 가장 먼 거리 
    AceLight.Diffuse = D3DXCOLOR(DXCOLOR_WHITE); // 광원 색상 설정 - 광원에 반사될 때 출력되는 가장 주된 색
    AceLight.Ambient = D3DXCOLOR(DXCOLOR_DARK_ORANGE); // 광원 주변 환경색 설정 - 광원의 위치와 무관하게 똑같은 양으로 모든 점에서 반사되는 색
    AceLight.Falloff = 1.f;
    // POINT 점광원 에서는 광선의 방향(DIRECTION) 무시 

    // 감쇠 - 거리에 따른 선형 감쇠값
    AceLight.Attenuation0 = 0.1f; // 정적 감쇠 계수 
    AceLight.Attenuation1 = 0.1f; // 선형 감쇠 계수 -> 방향성 광원인경우
    AceLight.Attenuation2 = 0.0f; // 2차 감쇠 계수 

    // 광원 등록
    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &AceLight, 4), E_FAIL);

    return S_OK;
}

