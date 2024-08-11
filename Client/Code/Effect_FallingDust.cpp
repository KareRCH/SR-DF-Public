#include "stdafx.h"
#include "Effect_FallingDust.h"
#include "AceMonster.h"

CEffect_FallingDust::CEffect_FallingDust(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_FallingDust::CEffect_FallingDust(const CEffect_FallingDust& rhs)
	:Base(rhs)
{
}

CEffect_FallingDust::~CEffect_FallingDust()
{
}

CEffect_FallingDust* CEffect_FallingDust::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - FallingDust Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CEffect_FallingDust::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"FallingDust");
	m_pTransformComp->Set_Scale({ 0.7f, 0.7f, 1.f });
	m_pTransformComp->Set_PosY(0.f);

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 10.f;
	m_tFrame.fFrame = 0;

	return S_OK;
}

_int CEffect_FallingDust::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();


	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	if (m_tFrame.fFrame >= m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = m_tFrame.fFrameEnd - 1.f;
		Set_Dead();
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_FallingDust::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_FallingDust::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_FallingDust::Add_Component()
{
	return S_OK;
}

void CEffect_FallingDust::Free()
{
	SUPER::Free();
}
