#include "CubeBufferComp.h"

CCubeBufferComp::CCubeBufferComp()
{
}

CCubeBufferComp::CCubeBufferComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CCubeBufferComp::CCubeBufferComp(const CCubeBufferComp& rhs)
	: Base(rhs)
	, m_pVertexPos(rhs.m_pVertexPos)
{
}

CCubeBufferComp::~CCubeBufferComp()
{
}

CCubeBufferComp* CCubeBufferComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Component()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCubeBufferComp Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CCubeBufferComp::Clone(void)
{
	return new ThisClass(*this);
}

void CCubeBufferComp::Free(void)
{
	if (false == m_bClone)
		Safe_Delete_Array(m_pVertexPos);

	SUPER::Free();
}

HRESULT CCubeBufferComp::Ready_Component()
{
	m_dwFVF = FVF_CUBE;
	m_dwVtxCnt = 8;
	m_dwTriCnt = 12;
	m_dwVtxSize = sizeof(VTXCUBE);

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	m_pVertexPos = new _vec3[m_dwVtxCnt];

	FAILED_CHECK_RETURN(CVIBufferComp::Ready_Buffer(), E_FAIL);

	VTXCUBE* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = m_pVertexPos[0] = {-0.5f, 0.5f, 0.5f };
	pVertex[0].vTexUV = pVertex[0].vPosition;

	pVertex[1].vPosition = m_pVertexPos[1] = { 0.5f, 0.5f, 0.5f };
	pVertex[1].vTexUV = pVertex[1].vPosition;

	pVertex[2].vPosition = m_pVertexPos[2] = { 0.5f, -0.5f, 0.5f };
	pVertex[2].vTexUV = pVertex[2].vPosition;

	pVertex[3].vPosition = m_pVertexPos[3] = { -0.5f, -0.5f, 0.5f };
	pVertex[3].vTexUV = pVertex[3].vPosition;

	pVertex[4].vPosition = m_pVertexPos[4] = { -0.5f, 0.5f, -0.5f };
	pVertex[4].vTexUV = pVertex[4].vPosition;

	pVertex[5].vPosition = m_pVertexPos[5] = { 0.5f, 0.5f, -0.5f };
	pVertex[5].vTexUV = pVertex[5].vPosition;

	pVertex[6].vPosition = m_pVertexPos[6] = { 0.5f, -0.5f, -0.5f };
	pVertex[6].vTexUV = pVertex[6].vPosition;

	pVertex[7].vPosition = m_pVertexPos[7] = { -0.5f, -0.5f, -0.5f };
	pVertex[7].vTexUV = pVertex[7].vPosition;

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// X+
	// 오른쪽 위
	pIndex[0]._0 = 1;
	pIndex[0]._1 = 5;
	pIndex[0]._2 = 6;

	// 왼쪽 아래 삼각형
	pIndex[1]._0 = 1;
	pIndex[1]._1 = 6;
	pIndex[1]._2 = 2;

	// X-
	// 오른쪽 위
	pIndex[2]._0 = 4;
	pIndex[2]._1 = 0;
	pIndex[2]._2 = 3;

	// 왼쪽 아래 삼각형
	pIndex[3]._0 = 4;
	pIndex[3]._1 = 3;
	pIndex[3]._2 = 7;

	// Y+
	// 오른쪽 위
	pIndex[4]._0 = 4;
	pIndex[4]._1 = 5;
	pIndex[4]._2 = 1;

	// 왼쪽 아래 삼각형
	pIndex[5]._0 = 4;
	pIndex[5]._1 = 1;
	pIndex[5]._2 = 0;

	// Y-
	// 오른쪽 위
	pIndex[6]._0 = 3;
	pIndex[6]._1 = 2;
	pIndex[6]._2 = 6;

	// 왼쪽 아래 삼각형
	pIndex[7]._0 = 3;
	pIndex[7]._1 = 6;
	pIndex[7]._2 = 7;

	// Z+
	// 오른쪽 위
	pIndex[8]._0 = 7;
	pIndex[8]._1 = 6;
	pIndex[8]._2 = 5;

	// 왼쪽 아래 삼각형
	pIndex[9]._0 = 7;
	pIndex[9]._1 = 5;
	pIndex[9]._2 = 4;

	// Z-
	// 오른쪽 위
	pIndex[10]._0 = 0;
	pIndex[10]._1 = 1;
	pIndex[10]._2 = 2;

	// 왼쪽 아래 삼각형
	pIndex[11]._0 = 0;
	pIndex[11]._1 = 2;
	pIndex[11]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CCubeBufferComp::LateUpdate_Component()
{
}

void CCubeBufferComp::Render_Component()
{
	SUPER::Render_Buffer();


}