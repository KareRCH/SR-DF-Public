#include "stdafx.h"
#include "UI_Hagrid.h"

CUI_Hagrid::CUI_Hagrid(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI_Hagrid::CUI_Hagrid(const CUI_Hagrid& rhs)
	: Base(rhs)
{
}

CUI_Hagrid::~CUI_Hagrid()
{
}

CUI_Hagrid* CUI_Hagrid::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("UI - Hagrid Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CUI_Hagrid::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 텍스쳐 크기 
	m_fSizeX = 300.f;
	m_fSizeY = 400.f;

	// 텍스쳐 중점위치 
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	// 640 dp x반절 150 = 
	m_pTextureComp->Set_Pos({ 750.f , 0.f, 0.f });	// 이미지 위치
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// 이미지 크기

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Panel", L"Journal"); // 이미지 

	// 수명 및 프레임 설정 
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 5.f;
	m_tFrame.fRepeat = 0.f;

	return S_OK;
}

HRESULT CUI_Hagrid::Add_Component()
{
	return S_OK;
}

_int CUI_Hagrid::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_fCX = m_pTextureComp->Get_Pos().x;
	
	if (!_bLeft)
	{
		m_fCX -= 2.f;
		m_pTextureComp->Set_Pos({ m_fCX , 0.f, 0.f });
		
		if (m_fCX <= 490.f)
			_bLeft = true;
	}

	m_tFrame.fAge += fTimeDelta;

	if (m_tFrame.fAge > m_tFrame.fLifeTime)
		_bRight = true;

	if(_bRight)
	{
		m_fCX += 2.f;
		m_pTextureComp->Set_Pos({ m_fCX , 0.f, 0.f });

		if (m_fCX <= 750.f)
			Set_Dead();
	}

	
	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_Hagrid::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CUI_Hagrid::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();
}

void CUI_Hagrid::Free()
{
	SUPER::Free();
}