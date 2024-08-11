#include "stdafx.h"
#include "Effect_DazeSwirl.h"

CEffect_DazeSwirl::CEffect_DazeSwirl(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEffect_DazeSwirl::CEffect_DazeSwirl(const CEffect_DazeSwirl& rhs)
	:Base(rhs)
{
}

CEffect_DazeSwirl::~CEffect_DazeSwirl()
{
}

CEffect_DazeSwirl* CEffect_DazeSwirl::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Effect - DazeSwirl Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y + 1.f, _z);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CEffect_DazeSwirl::Add_Component()
{
	return S_OK;
}

HRESULT CEffect_DazeSwirl::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	Owner_Dead(m_pOwner);

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"DazeSwirl");
	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });

	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 7.f;
	m_tFrame.fFrame = 0;

	return S_OK;
}

_int CEffect_DazeSwirl::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();

	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	if (m_tFrame.fFrame >= m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0;
		m_tFrame.fRepeat += 1.f;
	}

	if (4 == m_tFrame.fRepeat)
		Set_Dead();

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CEffect_DazeSwirl::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEffect_DazeSwirl::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CEffect_DazeSwirl::Free()
{
	SUPER::Free();
}
