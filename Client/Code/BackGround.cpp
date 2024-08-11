#include "stdafx.h"
#include "BackGround.h"

#include "Export_System.h"
#include "Export_Utility.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: Base(rhs)
{
}

CBackGround::~CBackGround()
{
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BackGround Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CBackGround::Add_Component()
{
	// Ʈ������ ������Ʈ
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	// ����
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	NULL_CHECK_RETURN(m_pBackTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TitleTexture", L"Proto_TitleBackTextureComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_LogoTexture", L"Proto_LogoTextureComp"), E_FAIL);
	
	

	return S_OK;
}

HRESULT CBackGround::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	Engine::Play_BGM(L"FallenAces", L"Scene_Logo.wav", 0.75f);
	Set_ObjectName(L"Background");

	m_fSizeX = WINCX;
	m_fSizeY = WINCY;

	m_fX = m_fSizeX * 0.5f; // ������ġ 
	m_fY = m_fSizeY * 0.5f;

	m_pTransformComp->Set_Pos({ m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f });	// �̹��� ��ġ
	m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });						// �̹��� ũ��


	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CBackGround::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CBackGround::Render_GameObject()
{
	// �ؽ�ó�� Ʈ�������� ���� ���, DYNAMIC���� ������ �������� ���� �ʿ䰡 ����
	m_pBackTextureComp->Readjust_Transform();	
	// �ؽ�ó ��� * �θ�(���ӿ�����Ʈ)�� ���
	m_pBackTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// ���� �ΰ��� ���� �ؽ�ó ��İ� �θ� ����� ������ �ΰ� ����� �� ����.

	// �������� Render_Texture �Լ� �ȿ��� �ڵ����� �ؽ�ó�� ����� ����̽��� ����.(SetTransform(D3DTS_WORLD, �ؽ�ó ���))
	m_pBackTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();

	// �̰� �θ� ����� �ؽ�ó ��Ŀ� �״�� ���� ���, �ؽ�ó ������ ����� �ʿ���� �� ���
	m_pTextureComp->Set_Transform(m_pTransformComp->Get_Transform());

	// m_pBackTextureComp�� ������ �Ͱ� m_pTextureComp�� ������ ��. �ΰ��� ��׶��忡�� ������ ��ġ, ũ��� �����ǹǷ� ��ȿ��.

	// Set_Transform : �ܼ� ��� �����
	// Readjust_Transform : �ؽ�ó�� ���� ��ǥ, ȸ��, ũ�⸦ �ؽ�ó�� ���� ���(Ʈ������)�� �����Ѵ�.
	// Set_TransformToWorld : �ؽ�ó ���� ��� * �θ� ���, ��İ���

	m_pTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();
}

void CBackGround::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CBackGround::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CBackGround::OnCollisionExited(CGameObject* pDst)
{
}

void CBackGround::Free()
{
	SUPER::Free();


}
