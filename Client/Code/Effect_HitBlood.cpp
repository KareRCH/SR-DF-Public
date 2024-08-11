#include "stdafx.h"
#include "Effect_HitBlood.h"

CEffect_HitBlood::CEffect_HitBlood(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_HitBlood::CEffect_HitBlood(const CEffect_HitBlood& rhs)
	:Base(rhs)
{
}

CEffect_HitBlood::~CEffect_HitBlood()
{
}

CEffect_HitBlood* CEffect_HitBlood::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - HitBlood Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CEffect_HitBlood::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"HitBlood");
	m_pTransformComp->Set_Scale({ 0.5f, 0.5f, 1.f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 7.f;
	m_tFrame.fFrame = 0;

	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 1.f;

	m_bTrigger = false;
	m_fVelocity = { 0.f, 2.f, -0.2f };

	_float RndY = Get_RandomFloat(-2.f, 6.f);
	m_fVelocity.x = Get_RandomFloat(-2.f, 2.f);
	m_fVelocity.y += RndY;
	m_fVelocity.z = Get_RandomFloat(-0.8f, 3.5f);

	m_fAcceleration = { 0.f, -30.f, 0.f };

	return S_OK;
}

_int CEffect_HitBlood::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	if (m_tFrame.fFrame >= m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = m_tFrame.fFrameEnd - 1.f;
		Set_Dead();
	}

	Billboard();

	m_pTransformComp->Move_Pos(&m_fVelocity, fTimeDelta, 1.f);

	m_fVelocity += m_fAcceleration * fTimeDelta;

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_HitBlood::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_HitBlood::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_HitBlood::Add_Component()
{
	return S_OK;
}

void CEffect_HitBlood::Free()
{
	SUPER::Free();
}
