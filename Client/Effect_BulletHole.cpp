#include "stdafx.h"
#include "Effect_BulletHole.h"

CEffect_BulletHole::CEffect_BulletHole(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_BulletHole::CEffect_BulletHole(const CEffect_BulletHole& rhs)
	:Base(rhs)
{
}

CEffect_BulletHole::~CEffect_BulletHole()
{
}

CEffect_BulletHole* CEffect_BulletHole::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
	_float _x, _float _y, _float _z, STATE_RIGHTHAND pHand)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - BulletHole Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->GunType = pHand;

	return pInstance;
}

HRESULT CEffect_BulletHole::Add_Component()
{
	return S_OK;
}

HRESULT CEffect_BulletHole::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	if(STATE_RIGHTHAND::GUN == GunType)
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"BulletHoles1");
	else if(STATE_RIGHTHAND::THOMPSON == GunType)
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"BulletHoles2");

	return S_OK;
}

_int CEffect_BulletHole::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_BulletHole::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_BulletHole::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture();
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CEffect_BulletHole::Free()
{
	SUPER::Free();
}
