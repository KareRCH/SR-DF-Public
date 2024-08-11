#include "stdafx.h"
#include "UI_CrossHair.h"

CUI_CrossHair::CUI_CrossHair(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI_CrossHair::CUI_CrossHair(const CUI_CrossHair& rhs)
	: Base(rhs)
{
}

CUI_CrossHair::~CUI_CrossHair()
{
}

CUI_CrossHair* CUI_CrossHair::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject())) {
		Safe_Release(pInstance);

		MSG_BOX("UI - CrossHair Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CUI_CrossHair::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 텍스쳐 크기 
	m_fSizeX = 60.f;
	m_fSizeY = 60.f;

	// 텍스쳐 중점위치 
	m_fX = m_fSizeX * 0.5f ;
	m_fY = m_fSizeY * 0.5f ;

	m_pTextureComp->Set_Pos({ 0.f , 0.f, 0.f });	// 이미지 위치
	m_pTextureComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });							// 이미지 크기

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Basic");

	m_bPlayerEquipGunState = false;
	m_bPlayerAttackState = false;
	m_bPlayerPickUpState = false;
	m_bPlayerDetect = false;
	HairSpin = false;
	return S_OK;
}

_int CUI_CrossHair::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Update_InternalData();

	Change_Texture();

	//if (m_bPlayerAttackState)
	//{
	//	for (double i = 0; i <= D3DX_PI * 2 ; i += fTimeDelta*0.1)
	//		m_pTextureComp->Set_RotationZ(i);

	//	HairSpin = false;
	//}

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_CrossHair::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CUI_CrossHair::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();
}

HRESULT CUI_CrossHair::Add_Component()
{
	return S_OK;
}

void CUI_CrossHair::Change_Texture()
{

	////로직짜야함 
	//if(m_bPlayerEquipGunState)
	//	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Gun");
	//else if (m_bPlayerEquipGunState)
	//		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Hit");
	//else if (m_bPlayerEquipGunState)
	//	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Windup");
	//else if (m_bPlayerEquipGunState)
	//	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Charge");

	if (m_bPlayerAttackState)
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Attack");
	else
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_Crosshair", L"Basic");
}

void CUI_CrossHair::Update_InternalData()
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
	m_bPlayerAttackState = pBlackBoard->Get_AttackOn();
	//m_fHp = pBlackBoard->Get_HP().Cur;
}

void CUI_CrossHair::Free()
{
	SUPER::Free();
}