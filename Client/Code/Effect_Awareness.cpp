#include "stdafx.h"
#include "Effect_Awareness.h"
#include "AceMonster.h"

CEffect_Awareness::CEffect_Awareness(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_Awareness::CEffect_Awareness(const CEffect_Awareness& rhs)
	:Base(rhs)
{
}

CEffect_Awareness::~CEffect_Awareness()
{
}

CEffect_Awareness* CEffect_Awareness::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CEffect_Awareness::TYPE pType, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - Awareness Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Owner_Get_Awareness(pOwner);
	pInstance->m_eType = pType;

	return pInstance;
}

HRESULT CEffect_Awareness::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");
	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrame = 0;

	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 1.f;

	m_fAwareness = 0.f;
	m_fMaxAwareness = 15.f;

	return S_OK;
}

_int CEffect_Awareness::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	Billboard();

	m_fAwarenessPrev = m_fAwareness;

	Owner_Get_Awareness(m_pOwner); // m_fAwareness 갱신 

	if(m_fAwarenessPrev > m_fAwareness) // 감소할때 
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"MissTarget");

	if (m_fAwarenessPrev < m_fAwareness) // 증가할때 
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");

	if (m_fAwareness >= m_fMaxAwareness)
	{
		m_fAwareness = m_fMaxAwareness - 1;
		m_bTrigger = TRUE;
	}
	
	if (m_bTrigger) // 변수 다 차면 이미지 바꾸고 1초뒤 삭제용 
	{
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"MaxAware");

		m_tFrame.fAge += fTimeDelta * 1.f;

		if (m_tFrame.fAge > m_tFrame.fLifeTime)
		{
			Set_Dead();
		}
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_Awareness::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}


void CEffect_Awareness::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	if (m_bTrigger)
		m_pTextureComp->Render_Texture();
	else
		m_pTextureComp->Render_Texture(_ulong(m_fAwareness));

	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_Awareness::Add_Component()
{
	return S_OK;
}

void CEffect_Awareness::Owner_Get_Awareness(CGameObject* pDst)
{
	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

	if (pAceObj == nullptr)
		return;
	else
	{
		CAceMonster* pMonster = dynamic_cast<CAceMonster*>(pAceObj);
		m_fAwareness = pMonster->Get_Awareness();
	}
}

void CEffect_Awareness::Free()
{
	SUPER::Free();
}