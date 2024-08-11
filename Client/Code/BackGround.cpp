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
	// 트랜스폼 컴포넌트
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	// 버퍼
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	// 텍스쳐 컴포넌트
	NULL_CHECK_RETURN(m_pBackTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TitleTexture", L"Proto_TitleBackTextureComp"), E_FAIL);
	// 텍스쳐 컴포넌트
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

	m_fX = m_fSizeX * 0.5f; // 중점위치 
	m_fY = m_fSizeY * 0.5f;

	m_pTransformComp->Set_Pos({ m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f });	// 이미지 위치
	m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });						// 이미지 크기


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
	// 텍스처의 트랜스폼을 월드 행렬, DYNAMIC으로 설정시 수동으로 해줄 필요가 없음
	m_pBackTextureComp->Readjust_Transform();	
	// 텍스처 행렬 * 부모(게임오브젝트)의 행렬
	m_pBackTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// 위의 두개만 쓰면 텍스처 행렬과 부모 행렬을 별개로 두고 계산할 수 있음.

	// 이제부터 Render_Texture 함수 안에서 자동으로 텍스처의 행렬이 디바이스에 들어간다.(SetTransform(D3DTS_WORLD, 텍스처 행렬))
	m_pBackTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();

	// 이건 부모 행렬을 텍스처 행렬에 그대로 쓰는 방법, 텍스처 별개의 행렬이 필요없을 때 사용
	m_pTextureComp->Set_Transform(m_pTransformComp->Get_Transform());

	// m_pBackTextureComp에 적용한 것과 m_pTextureComp에 적용한 것. 두개다 백그라운드에서 동일한 위치, 크기로 설정되므로 유효함.

	// Set_Transform : 단순 행렬 덮어쓰기
	// Readjust_Transform : 텍스처의 로컬 좌표, 회전, 크기를 텍스처의 월드 행렬(트랜스폼)에 적용한다.
	// Set_TransformToWorld : 텍스처 월드 행렬 * 부모 행렬, 행렬곱임

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
