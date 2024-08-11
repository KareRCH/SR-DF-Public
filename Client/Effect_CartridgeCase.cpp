#include "stdafx.h"
#include "Effect_CartridgeCase.h"

CEffect_CartridgeCase::CEffect_CartridgeCase(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_CartridgeCase::CEffect_CartridgeCase(const CEffect_CartridgeCase& rhs)
	:Base(rhs)
{
}

CEffect_CartridgeCase::~CEffect_CartridgeCase()
{
}

CEffect_CartridgeCase* CEffect_CartridgeCase::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	_float _x, _float _y, _float _z, _vec3 vRight, CGameObject * pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - Cartridge Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y + 1.f, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->PlayerRight = vRight;

	return pInstance;
}

HRESULT CEffect_CartridgeCase::Add_Component()
{
	return S_OK;
}

HRESULT CEffect_CartridgeCase::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	Owner_Dead(m_pOwner);

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Cartridge");
	m_pTransformComp->Set_Scale({ 0.6f, 0.6f, 0.6f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 5.f;
	m_tFrame.fFrame = 0;

	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 3.f;

	D3DXVec3Normalize(&PlayerRight, &PlayerRight);

	m_fVelocity = {PlayerRight.x, 2.f, PlayerRight.z };
	m_fAcceleration = { 0.f, -15.f, 0.f };

	return S_OK;
}

_int CEffect_CartridgeCase::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();

	m_pTransformComp->Move_Pos(&m_fVelocity, fTimeDelta, 1.f);

	m_fVelocity += m_fAcceleration * fTimeDelta;
	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;
	m_tFrame.fAge += fTimeDelta;

	if (m_tFrame.fFrame >= m_tFrame.fFrameEnd)
		m_tFrame.fFrame = 0;


	if (m_tFrame.fAge > m_tFrame.fLifeTime)
		Set_Dead();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_CartridgeCase::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_CartridgeCase::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


void CEffect_CartridgeCase::Free()
{
	SUPER::Free();
}
