#include "stdafx.h"
#include "UI_PlayerFace.h"

CUI_PlayerFace::CUI_PlayerFace(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI_PlayerFace::CUI_PlayerFace(const CUI_PlayerFace& rhs)
	: Base(rhs)
{
}

CUI_PlayerFace::~CUI_PlayerFace()
{
}

CUI_PlayerFace* CUI_PlayerFace::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject())) {
		Safe_Release(pInstance);

		MSG_BOX("UI PlayerFace Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CUI_PlayerFace::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �̹��� ����
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Face", L"Healthy");

	// ��Ÿ�� ��Ʈ�� �� ���� �ʱ�ȭ 
	m_gPlayerHP.Max = 0.f;
	m_gPlayerHP.Cur = 0.f;

	// ������������ �׸��̹��� ���� 
	m_fSizeX = 195;
	m_fSizeY = 171;

	m_fX = m_fSizeX * 0.5f ; 
	m_fY = m_fSizeY * 0.5f ;

	m_pTextureComp->Set_Pos({ m_fX - WINCX * 0.5f - 35, -m_fY - 177.f , 0.f });	// �̹��� ��ġ
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// �̹��� ũ��

	// ������ �ʱ�ȭ 
	m_tFrame.fFrame = 0.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 1.f;

	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 1.5f;

	m_bPlayerHit = false;
	m_bTrigger = false;
	m_bFaceTurn = false;

	return S_OK;
}

HRESULT CUI_PlayerFace::Add_Component()
{
	return S_OK;
}

_int CUI_PlayerFace::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Update_InternalData(); // hp������Ʈ �ϰ� 

	Change_Texture(fTimeDelta); // ������Ʈ �Ѱɷ� �ؽ��� �ٲٱ� 

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_PlayerFace::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CUI_PlayerFace::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame), true);
	m_pBufferComp->Render_Buffer();
}

void CUI_PlayerFace::Change_Texture(const _float& fTimeDelta)
{
	if (m_gPlayerHP.Cur >= 0 && m_gPlayerHP.Cur <= 20) // 0 <= a <= 20 
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Face", L"BadlyHurt");
	}
	else if (m_gPlayerHP.Cur > 20 && m_gPlayerHP.Cur <= 40)  // 20 < a <= 40
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Face", L"ModeratelyHurt");
	}
	else if (m_gPlayerHP.Cur > 40 && m_gPlayerHP.Cur <= 60)  // 40 < a <= 60
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Face", L"Hurt");
	}
	else if (m_gPlayerHP.Cur > 60 && m_gPlayerHP.Cur <= 80) // 60 < a <= 80
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Face", L"SlightlyHurt");
	}
	else if (m_gPlayerHP.Cur > 80 && m_gPlayerHP.Cur <= 100)  // 80 < a <= 100
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Face", L"Healthy");
	}

	if (m_bPlayerHit)
	{
		m_tFrame.fLifeTime = 1.f;
		m_tFrame.fFrame = 4;
		m_bTrigger = FALSE;
		m_bFaceTurn = TRUE;
		//OutputDebugString(L"�� : ��Ʈ ON \n");
	}

	if (!m_bTrigger && !m_bFaceTurn)
	{
		//OutputDebugString(L"�� : �¿� ����  \n");
		if (Random_variable(50))
			m_tFrame.fFrame = 0.f;
		else
			m_tFrame.fFrame = 2.f;

		m_tFrame.fLifeTime = 1.5f;
		m_bTrigger = TRUE;
		m_bFaceTurn = FALSE;
	}

	if (m_bTrigger || m_bFaceTurn)
	{
		m_tFrame.fAge += 1.0f * fTimeDelta;

		if (m_tFrame.fAge > m_tFrame.fLifeTime && m_bTrigger && !m_bFaceTurn)
		{
			//OutputDebugString(L"�� : ���� ����  \n");
			// ���� �����϶�  LifeTime���� 
			m_tFrame.fAge = 0.0f;
			m_tFrame.fFrame = 1.0f; // �߰��� 1�� ����

			m_bTrigger = FALSE;
			m_bFaceTurn = TRUE;
		}

		if (m_tFrame.fAge > m_tFrame.fLifeTime && !m_bTrigger && m_bFaceTurn)
		{
			//OutputDebugString(L"�� : ������ �Ȱǵ�� �¿� ���°ɷ� ���ư���   \n");
			// ��� �����϶�  LifeTime���� 
			m_tFrame.fAge = 0.0f;
			m_bTrigger = FALSE;
			m_bFaceTurn = FALSE;
		}
	}

}

void CUI_PlayerFace::Update_InternalData()
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

	// ���⼭���� �������� ������ ���´�.
	m_gPlayerHP = pBlackBoard->Get_HP();
	m_bPlayerHit = pBlackBoard->Get_PlayerHit();
}

void CUI_PlayerFace::Free()
{
	SUPER::Free();
}
