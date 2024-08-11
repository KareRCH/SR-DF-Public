#include "RcColorComp.h"

CRcColorComp::CRcColorComp()
{
}

CRcColorComp::CRcColorComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CRcColorComp::CRcColorComp(const CRcColorComp& rhs)
	: Base(rhs)
{
}

CRcColorComp::~CRcColorComp()
{
}

HRESULT CRcColorComp::Ready_Buffer()
{
#pragma region 버텍스 버퍼 방식

	//m_dwFVF = FVF_COL;
	//m_dwTriCnt = 2;
	//m_dwVtxCnt = 6;
	//m_dwVtxSize = sizeof(VTXCOL);

	//FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	//VTXCOL*		pVertex = nullptr;

	//m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	//// 1. 어디서부터 메모리를 잠글 것인가
	//// 2. 0인 경우 전체 영역을 잠금
	//// 3. 버텍스 버퍼에 저장된 정점들 중 첫 번째 주소를 반환
	//// 4. 잠그는 형태, 정적 버퍼인 경우는 0을 넣어줌

	//// LT (-1, 1) RB(1, -1)

	//pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	//pVertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	//pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	//pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	//pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	//pVertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	//pVertex[3].vPosition = { -1.f, 1.f, 0.f };
	//pVertex[3].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//pVertex[4].vPosition = { 1.f, -1.f, 0.f };
	//pVertex[4].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//pVertex[5].vPosition = { -1.f, -1.f, 0.f };
	//pVertex[5].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//m_pVB->Unlock();
#pragma endregion 버텍스 버퍼 방식

#pragma region 인덱스 버퍼 방식
	m_dwFVF = FVF_COL;
	m_dwTriCnt = 2;
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXCOL);

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBufferComp::Ready_Buffer(), E_FAIL);

	VTXCOL* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 1. 어디서부터 메모리를 잠글 것인가
	// 2. 0인 경우 전체 영역을 잠금
	// 3. 버텍스 버퍼에 저장된 정점들 중 첫 번째 주소를 반환
	// 4. 잠그는 형태, 정적 버퍼인 경우는 0을 넣어줌

	// LT (-1, 1) RB(1, -1)

	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[3].vPosition = { -1.f, -1.f, 0.f };
	pVertex[3].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

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
#pragma endregion 인덱스 버퍼 방식

	return S_OK;
}

void CRcColorComp::Render_Buffer()
{
	SUPER::Render_Buffer();
}

CRcColorComp* CRcColorComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcColorComp* pInstance = new CRcColorComp(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("RcCol Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CRcColorComp::Clone()
{
	// 복사 생성
	return new CRcColorComp(*this);
}

void CRcColorComp::Free()
{
	SUPER::Free();
}

HRESULT CRcColorComp::Ready_Component()
{
	return E_NOTIMPL;
}

void CRcColorComp::LateUpdate_Component()
{
}

void CRcColorComp::Render_Component()
{
}
