#include "stdafx.h"
#include "UI_Player.h"

#include "DynamicCamera.h"

CUI_Player::CUI_Player(LPDIRECT3DDEVICE9 pGraphicDev) 
	: Base(pGraphicDev)
{
}

CUI_Player::CUI_Player(const CUI_Player& rhs)
	: Base(rhs)
{
}

CUI_Player::~CUI_Player()
{
}

CUI_Player* CUI_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject())) {
		Safe_Release(pInstance);

		MSG_BOX("UI Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CUI_Player::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 이미지 설정
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI", L"PlayerHudBox");
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrame = 0.f;

	// 나타낼 폰트에 들어갈 변수 초기화 
	m_gPlayerHP.Max = 0.f;
	m_gPlayerHP.Cur = 0.f;

	// 직교투영으로 그릴이미지 셋팅 
	m_fSizeX = 250;
	m_fSizeY = 250;

	m_fX = m_fSizeX * 0.5f; // 중점위치 
	m_fY = m_fSizeY * 0.5f + WINCY - m_fSizeY;

	m_pTextureComp->Set_Pos({ m_fX - WINCX * 0.5f - 20.f, -m_fY + WINCY * 0.5f - 45.f  , 0.f });	// 이미지 위치
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// 이미지 크기

	return S_OK;
}

HRESULT CUI_Player::Add_Component()
{
	return S_OK;
}

void CUI_Player::Update_InternalData()
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

	// 여기서부터 블랙보드의 정보를 얻어온다.
	m_gPlayerHP = pBlackBoard->Get_HP();
}

_int CUI_Player::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Update_InternalData();

	if (Engine::IsMouse_Pressed(DIM_MWU))
		m_tFrame.fFrame -= 1.f;

	if (Engine::IsMouse_Pressed(DIM_MWD))
		m_tFrame.fFrame += 1.f;

	if (m_tFrame.fFrame >= m_tFrame.fFrameEnd)
		m_tFrame.fFrame = m_tFrame.fFrameEnd-1.f;

	if(m_tFrame.fFrame <= 0)
		m_tFrame.fFrame = 0.f;

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_Player::LateUpdate_GameObject()
{
}

void CUI_Player::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame), true);
	m_pBufferComp->Render_Buffer();

	_vec2 vFontPos = { 40.f, WINCY - 35.f };
	wstringstream ss;
	ss << m_gPlayerHP.Cur;
	Engine::Render_Font(L"Font_Thin_Jinji", ss.str().c_str(), &vFontPos, DXCOLOR_RED);
}

void CUI_Player::Free()
{
	SUPER::Free();
}
