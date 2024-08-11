#include "stdafx.h"
#include "UI_MonsterHP.h"
#include "Gray.h"
#include "Brown.h"
#include "AceBoss.h"

CUI_MonsterHP::CUI_MonsterHP(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP& rhs)
	: Base(rhs)
{
}

CUI_MonsterHP::~CUI_MonsterHP()
{
}

CUI_MonsterHP* CUI_MonsterHP::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject())) {
		Safe_Release(pInstance);

		MSG_BOX("UI - Monster HP Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CUI_MonsterHP::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//�ʱ� ���� ����
	m_gMonsterHP.Cur = 100.f;
	m_gMonsterHP.Max = 100.f;


	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 2.f;

	// �̹��� ����
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"UI_HpBar", L"Border");
	m_pHPTextureComp->Receive_Texture(TEX_NORMAL, L"UI_HpBar", L"Health");

	// ������������ �׸��̹��� ���� 
	m_fBarSizeX = m_gMonsterHP.Max *3.f; // �ܰ� �׵θ�
	m_fBarSizeY = 35.f;

	m_fRedSizeX = m_gMonsterHP.Cur * 2.f; // ���� hp
	m_fRedSizeY = 17.f;

	// ������ġ
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	//// �̹��� ��ġ
	m_pTextureComp->Set_Pos({ 0.f , WINCY * 0.5 - 65.f, 0.f });	
	m_pTextureComp->Set_Scale({ m_fBarSizeX, m_fBarSizeY, 1.f });							// �̹��� ũ��

	m_pHPTextureComp->Set_Pos({ 0.f , WINCY * 0.5 - 67.f, 0.f });
	m_pHPTextureComp->Set_Scale({ m_fRedSizeX, m_fRedSizeY, 1.f });							// �̹��� ũ��

	return S_OK;
}

HRESULT CUI_MonsterHP::Add_Component()
{
	//�߰� �ؽ��� 
	NULL_CHECK_RETURN(m_pHPTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_TextureHP", L"Proto_UITextureComp"), E_FAIL);
	
	return S_OK;
}


_int CUI_MonsterHP::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Update_InternalData();

	m_fRedSizeX = m_gMonsterHP.Cur * 3.f; // ���� hp
	m_pHPTextureComp->Set_Scale({ m_fRedSizeX, m_fRedSizeY, 1.f });
	m_pHPTextureComp->Set_Pos({ 0.f - (m_gMonsterHP.Max - m_gMonsterHP.Cur)*1.5f , WINCY * 0.5 - 67.f, 0.f});
	
	m_tFrame.fAge += fTimeDelta;

	if (m_tFrame.fAge > m_tFrame.fLifeTime)
		Set_Dead();


	// ==== ����� ���� ========
	//swprintf_s(debugString, L"��UI - HPüũ  = %f \n", m_gMonsterHP.Cur);
	//OutputDebugStringW(debugString);
	//
	//swprintf_s(debugStringOwner, L"�ڡڡ�Monster Ownerüũ = %s \n", m_sOwner.c_str());
	//OutputDebugStringW(debugStringOwner);


	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_MonsterHP::LateUpdate_GameObject()
{
}

void CUI_MonsterHP::Render_GameObject()
{
	m_pTextureComp->Readjust_Transform();
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTextureComp->Get_Transform());
	m_pTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();
	

	m_pHPTextureComp->Readjust_Transform();
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pHPTextureComp->Get_Transform());
	m_pHPTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();

	
	
	Engine::Render_Font(L"MonsterUI", m_sOwner.c_str(), &vFontPos, DXCOLOR_LIGHT_GRAY);

}

void CUI_MonsterHP::Free()
{
	SUPER::Free();
}

void CUI_MonsterHP::Update_InternalData()
{
	// ������ ���� ���, ���� �ڵ�� �ʼ�
	if (!m_wpBlackBoard_Monster.Get_BlackBoard())
	{
		m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
		// ���� ����
		if (!m_wpBlackBoard_Monster.Get_BlackBoard())
			return;
	}

	// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

	// ���⼭���� �������� ������ ���´�.
	m_gMonsterHP = pBlackBoard->Get_MonsterHP();
	m_sOwner = pBlackBoard->Get_Owner();

	if(m_sOwner.compare(L"Unarmed Goon") == 0)
		vFontPos = { WINCX * 0.5f - 80.f, 28.f };
	else if (m_sOwner.compare(L"Pipe Wielding Mook") == 0)
		vFontPos = { WINCX * 0.5f - 130.f, 28.f };
	else //����
		vFontPos = { WINCX * 0.5f - 50.f, 28.f };

	//Reveal_Owner(m_pHPOwner);
}
