#include "stdafx.h"
#include "Effect_HitPow.h"
#include "AceMonster.h"

CEffect_HitPow::CEffect_HitPow(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_HitPow::CEffect_HitPow(const CEffect_HitPow& rhs)
	:Base(rhs)
{
}

CEffect_HitPow::~CEffect_HitPow()
{
}

CEffect_HitPow* CEffect_HitPow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - HitPow Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CEffect_HitPow::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"HitPow");
	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 6.f;
	m_tFrame.fFrame = 0;

	return S_OK;
}

_int CEffect_HitPow::Update_GameObject(const _float& fTimeDelta)
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

void CEffect_HitPow::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_HitPow::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_HitPow::Update_Pos()
{
	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(m_pOwner);

	if (pAceObj == nullptr)
		return 0;
	else
	{
		CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);
		_vec3 OwnerPos = pMonster->Get_MonsterPos();
	}

	//이펙트가 플레이어 보는쪽 
	ResultDir = EffectPos - OwnerPos;
	
	//정규화 
	D3DXVec3Normalize(&ResultDir, &ResultDir); 
	
	EffectPos += ResultDir * 1.5f;

	m_pTransformComp->Set_Pos(ResultDir);

	return S_OK;
}
HRESULT CEffect_HitPow::Add_Component()
{
	return S_OK;
}

void CEffect_HitPow::Free()
{
	SUPER::Free();
}
