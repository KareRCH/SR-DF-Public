#include "stdafx.h"
#include "..\Header\SkyBox.h"

#include "Export_System.h"
#include "Export_Utility.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: Engine::CGameObject(rhs)
{
}

CSkyBox::~CSkyBox()
{
}

HRESULT CSkyBox::Add_Component()
{
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_SkyBoxTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_CubeBufferComp"), E_FAIL);

	return S_OK;
}

CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fScale)
{
	CSkyBox* pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("SkyBox Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Scale({ _fScale, _fScale, _fScale });

	return pInstance;
}


void CSkyBox::Free()
{
	SUPER::Free();
}

HRESULT CSkyBox::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

Engine::_int CSkyBox::Update_GameObject(const _float& fTimeDelta)
{
	_matrix	matCamWorld;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, nullptr, &matCamWorld);

	m_pTransformComp->Set_Pos(matCamWorld._41, matCamWorld._42 + 4.f, matCamWorld._43);

	Engine::Add_RenderGroup(RENDER_PRIORITY, this);

	return 	SUPER::Update_GameObject(fTimeDelta);
}

void CSkyBox::LateUpdate_GameObject()
{
	
}
void CSkyBox::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureComp->Render_Texture();
	m_pCubeBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}



