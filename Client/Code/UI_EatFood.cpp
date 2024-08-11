#include "stdafx.h"
#include "UI_EatFood.h"

CUI_EatFood::CUI_EatFood(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI_EatFood::CUI_EatFood(const CUI_EatFood& rhs)
	: Base(rhs)
{
}

CUI_EatFood::~CUI_EatFood()
{
}

CUI_EatFood* CUI_EatFood::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("UI - EatFood Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CUI_EatFood::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 텍스쳐 크기 
	m_fSizeX = WINCX;
	m_fSizeY = WINCY;

	// 텍스쳐 중점위치 
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTextureComp->Set_Pos({ 0.f , 0.f, 0.f });	// 이미지 위치
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// 이미지 크기

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Scene", L"EatEffect");

	// 수명 및 프레임 설정 
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 5.f;
	m_tFrame.fRepeat = 0.f;

	m_tFrame.fFrame = 0.f;
	m_tFrame.fFrameSpeed = 15.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	return S_OK;
}

HRESULT CUI_EatFood::Add_Component()
{
	return S_OK;
}

_int CUI_EatFood::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = m_tFrame.fFrameEnd - 1.f;

		Set_Dead();
	}

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_EatFood::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CUI_EatFood::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture((_ulong)m_tFrame.fFrame, true);
	m_pBufferComp->Render_Buffer();
}

void CUI_EatFood::Free()
{
	SUPER::Free();
}