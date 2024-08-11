#include "RcBufferComp.h"

CRcBufferComp::CRcBufferComp()
{

}

CRcBufferComp::CRcBufferComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{

}

CRcBufferComp::CRcBufferComp(const CRcBufferComp& rhs)
	: Base(rhs)
	, m_pVertexPos(rhs.m_pVertexPos)
{

}

CRcBufferComp::~CRcBufferComp()
{

}

CRcBufferComp* CRcBufferComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("RcTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CRcBufferComp::Clone()
{
	return new ThisClass(*this);
}

void CRcBufferComp::Free()
{	
	if (false == m_bClone)
		Safe_Delete_Array(m_pVertexPos);

	SUPER::Free();
}


HRESULT CRcBufferComp::Ready_Buffer()
{
	m_dwFVF = FVF_TEX;
	m_dwTriCnt = 2;
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXTEX);

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	m_pVertexPos = new _vec3[m_dwVtxCnt];

	FAILED_CHECK_RETURN(CVIBufferComp::Ready_Buffer(), E_FAIL);

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = m_pVertexPos[0] = { -0.5f, 0.5f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = m_pVertexPos[1] = { 0.5f, 0.5f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = m_pVertexPos[2] = { 0.5f, -0.5f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = m_pVertexPos[3] = { -0.5f, -0.5f, 0.f };
	pVertex[3].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래 삼각형
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CRcBufferComp::Render_Buffer()
{
	SUPER::Render_Buffer();
}

HRESULT CRcBufferComp::Ready_Component()
{
	return E_NOTIMPL;
}

void CRcBufferComp::LateUpdate_Component()
{
}

void CRcBufferComp::Render_Component()
{
}

