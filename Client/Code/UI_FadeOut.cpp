#include "stdafx.h"
#include "UI_FadeOut.h"

CUI_FadeOut::CUI_FadeOut(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI_FadeOut::CUI_FadeOut(const CUI_FadeOut& rhs)
	: Base(rhs)
{
}

CUI_FadeOut::~CUI_FadeOut()
{
}

CUI_FadeOut* CUI_FadeOut::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("UI - FadeOut Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CUI_FadeOut::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �ؽ��� ũ�� 
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;

	// �ؽ��� ������ġ 
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTextureComp->Set_Pos({ 0.f , 0.f, 0.f });	// �̹��� ��ġ
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// �̹��� ũ��

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Scene", L"FadeInOut");

	// ���� �� ������ ���� 
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 5.f;
	m_tFrame.fRepeat = 0.f;

	m_tFrame.fFrame = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = -40.f;
	m_tFrame.fFrameEnd = 0.f;
	return S_OK;
}

HRESULT CUI_FadeOut::Add_Component()
{
	return S_OK;
}

_int CUI_FadeOut::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	if (m_bEnd)
		Set_Dead();

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	if (m_tFrame.fFrame < m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0.f;
		m_bEnd = true;
	}

	Update_BlackBoard();

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_FadeOut::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();

}

void CUI_FadeOut::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture((_ulong)m_tFrame.fFrame, true);
	m_pBufferComp->Render_Buffer();
}

void CUI_FadeOut::Free()
{
	SUPER::Free();
}

void CUI_FadeOut::Update_BlackBoard()
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
	
	pBlackBoard->Get_SceneFade() = m_bEnd;
}