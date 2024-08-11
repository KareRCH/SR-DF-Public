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
#pragma region ���ؽ� ���� ���

	//m_dwFVF = FVF_COL;
	//m_dwTriCnt = 2;
	//m_dwVtxCnt = 6;
	//m_dwVtxSize = sizeof(VTXCOL);

	//FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	//VTXCOL*		pVertex = nullptr;

	//m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	//// 1. ��𼭺��� �޸𸮸� ��� ���ΰ�
	//// 2. 0�� ��� ��ü ������ ���
	//// 3. ���ؽ� ���ۿ� ����� ������ �� ù ��° �ּҸ� ��ȯ
	//// 4. ��״� ����, ���� ������ ���� 0�� �־���

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
#pragma endregion ���ؽ� ���� ���

#pragma region �ε��� ���� ���
	m_dwFVF = FVF_COL;
	m_dwTriCnt = 2;
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXCOL);

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBufferComp::Ready_Buffer(), E_FAIL);

	VTXCOL* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 1. ��𼭺��� �޸𸮸� ��� ���ΰ�
	// 2. 0�� ��� ��ü ������ ���
	// 3. ���ؽ� ���ۿ� ����� ������ �� ù ��° �ּҸ� ��ȯ
	// 4. ��״� ����, ���� ������ ���� 0�� �־���

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

	// ������ ��
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// ���� �Ʒ� �ﰢ��
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();
#pragma endregion �ε��� ���� ���

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
	// ���� ����
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
