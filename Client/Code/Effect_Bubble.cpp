#include "stdafx.h"
#include "Effect_Bubble.h"

CEffect_Bubble::CEffect_Bubble(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_Bubble::CEffect_Bubble(const CEffect_Bubble& rhs)
	:Base(rhs)
{
}

CEffect_Bubble::~CEffect_Bubble()
{
}

CEffect_Bubble* CEffect_Bubble::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - Bubble Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y + 1.f, _z);

	return pInstance;
}

HRESULT CEffect_Bubble::Add_Component()
{
	return S_OK;
}

HRESULT CEffect_Bubble::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	Owner_Dead(m_pOwner);

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Bubble");
	m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 0.5f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 12.f;
	m_tFrame.fFrame = 0;

	return S_OK;
}

_int CEffect_Bubble::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();

	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	if (m_tFrame.fFrame >= m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = m_tFrame.fFrameEnd-1.f ;
		
		Set_Dead();
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_Bubble::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_Bubble::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CEffect_Bubble::Free()
{
	SUPER::Free();
}
