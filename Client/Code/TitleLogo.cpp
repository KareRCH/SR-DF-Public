#include "stdafx.h"
#include "TitleLogo.h"

#include "Export_System.h"
#include "Export_Utility.h"

CTitleLogo::CTitleLogo(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTitleLogo::CTitleLogo(const CTitleLogo& rhs)
    : Base(rhs)
{
}

CTitleLogo::~CTitleLogo()
{
}

CTitleLogo* CTitleLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("TitleLogo Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CTitleLogo::Free()
{
	SUPER::Free();
}

HRESULT CTitleLogo::Add_Component()
{
	// 버퍼
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	// 텍스쳐 컴포넌트
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_LogoTextureComp"), E_FAIL);


	return S_OK;
}

HRESULT CTitleLogo::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTitleLogo::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CTitleLogo::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CTitleLogo::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pTextureComp->Render_Texture(1);
	m_pBufferComp->Render_Buffer();


	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
