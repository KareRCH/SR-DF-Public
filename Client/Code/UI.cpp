#include "stdafx.h"
#include "UI.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CUI::CUI(const CUI& rhs)
	: Base(rhs)
{

}

CUI::~CUI()
{

}

HRESULT CUI::Ready_GameObject()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	SUPER::Ready_GameObject();

	return S_OK;
}

_int CUI::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	return 0;
}

void CUI::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CUI::Free()
{
	SUPER::Free();
}