#include "TriColorComp.h"

CTriColorComp::CTriColorComp()
{

}

CTriColorComp::CTriColorComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{

}

CTriColorComp::CTriColorComp(const CTriColorComp& rhs)
	: Base(rhs)
{

}

CTriColorComp::~CTriColorComp()
{

}

HRESULT CTriColorComp::Ready_Buffer()
{
	m_dwFVF = FVF_COL;
	m_dwTriCnt = 1;
	m_dwVtxCnt = 3;
	m_dwVtxSize = sizeof(VTXCOL);

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(Base::Ready_Buffer(), E_FAIL);

	VTXCOL* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[2].vPosition = { 0.f, -1.f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();

	return S_OK;
}

void CTriColorComp::Render_Buffer()
{
	Base::Render_Buffer();
}

CTriColorComp* CTriColorComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("TriCol Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CTriColorComp::Clone()
{
	return new ThisClass(*this);
}

void CTriColorComp::Free()
{
	SUPER::Free();
}

HRESULT CTriColorComp::Ready_Component()
{
	return E_NOTIMPL;
}

void CTriColorComp::LateUpdate_Component()
{
}

void CTriColorComp::Render_Component()
{
}
