#include "TerrainBufferComp.h"

#include "Serialize_BaseClass.h"

#include "Export_System.h"
#include "Export_Utility.h"


CTerrainBufferComp::CTerrainBufferComp()
	: m_dwCountX(0U), m_dwCountZ(0U)
	, m_bHeightMap_Loaded(false)
	, m_pPos(nullptr)
	, m_vInvOffset(0.f, 0.f, 0.f), m_vScale(1.f, 1.f, 1.f)
{
}

CTerrainBufferComp::CTerrainBufferComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
	, m_dwCountX(0U), m_dwCountZ(0U)
	, m_bHeightMap_Loaded(false)
	, m_pPos(nullptr)
	, m_vInvOffset(0.f, 0.f, 0.f), m_vScale(1.f, 1.f, 1.f)
{
}

CTerrainBufferComp::CTerrainBufferComp(const CTerrainBufferComp& rhs)
	: Base(rhs)
	, m_dwCountX(rhs.m_dwCountX), m_dwCountZ(rhs.m_dwCountZ)
	, m_bHeightMap_Loaded(rhs.m_bHeightMap_Loaded)
	, m_vHeightMap(rhs.m_vHeightMap)
	, m_pPos(rhs.m_pPos)
	, m_vInvOffset(rhs.m_vInvOffset), m_vScale(rhs.m_vScale)
{
}

CTerrainBufferComp::~CTerrainBufferComp()
{
}

CTerrainBufferComp* CTerrainBufferComp::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
	const _tchar* pFileName, 
	const _ulong& dwCntX, const _ulong& dwCntZ, 
	const _vec3& vScale, const _vec3& vInvOffset)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(pFileName, dwCntX, dwCntZ, vScale, vInvOffset)))
	{
		Safe_Release(pInstance);
		MSG_BOX("TerrainTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CTerrainBufferComp* CTerrainBufferComp::Create(LPDIRECT3DDEVICE9 pGraphicDev, const char* pParsedFile, const _tchar* pFileName)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(pParsedFile, pFileName)))
	{
		Safe_Release(pInstance);
		MSG_BOX("TerrainTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CTerrainBufferComp::Clone()
{
	// ���� ����
	return new ThisClass(*this);
}

void CTerrainBufferComp::Free()
{
	if (!m_bClone)
		Safe_Delete_Array(m_pPos);

	SUPER::Free();
}


HRESULT CTerrainBufferComp::Ready_Buffer(const _tchar* pFileName, const _ulong& dwCntX, const _ulong& dwCntZ, const _vec3& vScale, const _vec3& vInvOffset)
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
	m_dwFVF = FVF_TEX;
	m_dwCountX = dwCntX;
	m_dwCountZ = dwCntZ;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;		// 128 x 128ũ���� ����
	m_dwVtxCnt = dwCntX * dwCntZ;						// 128 x 128ũ���� �� ����
	m_dwVtxSize = sizeof(VTXTEX);
	m_vScale = vScale;
	m_vInvOffset = vInvOffset;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	m_pPos = new _vec3[m_dwVtxCnt];

	

	// ���⼭ ������ �Է��� ������ ���� ������ ũ�⸦ Ȯ���Ѵ�.
	FAILED_CHECK_RETURN(CVIBufferComp::Ready_Buffer(), E_FAIL);

	// ���� ���� ���´�.
	if (pFileName != L"")
		Load_HeightMap(pFileName);
	else
	{
		m_vHeightMap.resize(m_dwVtxCnt);
		fill(m_vHeightMap.begin(), m_vHeightMap.end(), 0);
	}

	VTXTEX* pVertex = nullptr;

	if (m_pVB->Lock(0, 0, (void**)&pVertex, 0) == S_OK)
	{
		// 1. ��𼭺��� �޸𸮸� ��� ���ΰ�
		// 2. 0�� ��� ��ü ������ ���
		// 3. ���ؽ� ���ۿ� ����� ������ �� ù ��° �ּҸ� ��ȯ
		// 4. ��״� ����, ���� ������ ���� 0�� �־���

		// LT (-1, 1) RB(1, -1)
		_ulong dwIndex(0);
		for (_ulong i = 0; i < dwCntZ; i++)
		{
			for (_ulong j = 0; j < dwCntX; j++)
			{
				dwIndex = j + i * dwCntX;

				if (m_bHeightMap_Loaded)
					pVertex[dwIndex].vPosition = { (_float)j * m_vScale.x - m_vInvOffset.x,
													((_float)m_vHeightMap[dwIndex] / 255.f) * m_vScale.y - m_vInvOffset.y,
													(_float)i * m_vScale.z - m_vInvOffset.z };
				else
					pVertex[dwIndex].vPosition = { (_float)j, 0.f, (_float)i };

				m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

				pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };

				pVertex[dwIndex].vTexUV = { ((_float)j / ((_float)dwCntX - 1)) * 40.f * ((_float)m_dwCountX / 128.f),
											((_float)i / ((_float)dwCntZ - 1)) * 40.f * ((_float)m_dwCountX / 128.f) };
			}
		}

		INDEX32* pIndex = nullptr;
		_ulong		dwTriIdx(0);
		_vec3		vDst, vSrc, vNormal;

		//	Safe_Delete_Array(m_pPos);

		m_pIB->Lock(0, 0, (void**)&pIndex, 0);
		// ���� �¿�� 129������ 128���� �ٿ� ���
		for (_ulong i = 0; i < dwCntZ - 1; i++)
		{
			for (_ulong j = 0; j < dwCntX - 1; j++)
			{
				dwIndex = i * dwCntX + j;

				// ������ ��
				pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
				pIndex[dwTriIdx]._1 = dwIndex + dwCntX + 1;
				pIndex[dwTriIdx]._2 = dwIndex + 1;

				// �ﰢ���� �� ���� ���� ���ϱ�
				vDst = pVertex[pIndex[dwTriIdx]._1].vPosition - pVertex[pIndex[dwTriIdx]._0].vPosition;
				vSrc = pVertex[pIndex[dwTriIdx]._2].vPosition - pVertex[pIndex[dwTriIdx]._1].vPosition;
				D3DXVec3Cross(&vNormal, &vDst, &vSrc);	// ����

				// �� ���� ���� ���� ���� ����� �ֱ�, �̷� ���� ��ġ�� ���͵� �����Ѵ�. �̴� ����ȭ�� �ذ�.
				pVertex[pIndex[dwTriIdx]._0].vNormal += vNormal;
				pVertex[pIndex[dwTriIdx]._1].vNormal += vNormal;
				pVertex[pIndex[dwTriIdx]._2].vNormal += vNormal;

				dwTriIdx++;

				// ���� �Ʒ� �ﰢ��
				pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
				pIndex[dwTriIdx]._1 = dwIndex + 1;
				pIndex[dwTriIdx]._2 = dwIndex;

				vDst = pVertex[pIndex[dwTriIdx]._1].vPosition - pVertex[pIndex[dwTriIdx]._0].vPosition;
				vSrc = pVertex[pIndex[dwTriIdx]._2].vPosition - pVertex[pIndex[dwTriIdx]._1].vPosition;
				D3DXVec3Cross(&vNormal, &vDst, &vSrc);

				pVertex[pIndex[dwTriIdx]._0].vNormal += vNormal;
				pVertex[pIndex[dwTriIdx]._1].vNormal += vNormal;
				pVertex[pIndex[dwTriIdx]._2].vNormal += vNormal;

				dwTriIdx++;
			}
		}

		// ��� ���� ���� ����ȭ�ϱ�
		for (_ulong i = 0; i < m_dwVtxCnt; ++i)
			D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);

		m_pVB->Unlock();
		m_pIB->Unlock();
	}

	
#pragma endregion �ε��� ���� ���

	return S_OK;
}

HRESULT CTerrainBufferComp::Ready_Buffer(const char* pParsedFile, const _tchar* pHeightFile)
{
	FSerialize_Terrain tTerrain;
	string strJson;

	ifstream inputFile(pParsedFile);
	if (inputFile.is_open())
	{
		// ���ڿ� ���� �о���� �ݺ���
		strJson = string(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>());
		inputFile.close();
		cout << "\n���� �ҷ���!\n";
	}
	else
	{
		cerr << "������ �ҷ����� �� ����!\n";
	}

	tTerrain.Receive_ByRapidJSON(strJson);

	Ready_Buffer(pHeightFile, 
		static_cast<_ulong>(tTerrain.vVertexCount.x), static_cast<_ulong>(tTerrain.vVertexCount.z),
		tTerrain.vScale, tTerrain.vInvOffset);

	return S_OK;
}

void CTerrainBufferComp::Render_Buffer()
{
	SUPER::Render_Buffer();
}

HRESULT CTerrainBufferComp::Load_HeightMap(const _tchar* pFileName)
{
	m_vHeightMap.clear();   // ���� ���̸� �ʱ�ȭ
	m_vHeightMap.reserve(m_dwVtxCnt);
	BITMAPFILEHEADER bf = {};
	BITMAPINFOHEADER bInfo = {};

	// ���� ��Ʈ�� ����, ���� ���ÿ� ����
	ifstream fin(pFileName, ios_base::binary);

	// ����üũ - return
	if (!fin.is_open())
		return E_FAIL;

	// ���� ��� �б�
	fin.read((char*)&bf, sizeof(BITMAPFILEHEADER));
	// ����üũ - return
	if (bf.bfType == 0x424D)        // 0x424D�� bmp ��� �ĺ��ڵ�
		return E_FAIL;

	// bmp ��� �б�
	fin.read((char*)&bInfo, sizeof(BITMAPINFOHEADER));
	_int iPerByte = bInfo.biBitCount / 8;

	// data ���� �б�
	_ubyte* pData = new _ubyte[m_dwVtxCnt * iPerByte];
	fin.read((char*)pData, m_dwVtxCnt * iPerByte);

	_int r, g, b, mean;
	for (_int y = 0; y < bInfo.biHeight; y++)
	{
		for (_int x = 0; x < bInfo.biWidth; x++)
		{
			_int index = (x + y * (bInfo.biWidth)) * (iPerByte);

			b = pData[index];
			g = pData[index + 1];
			r = pData[index + 2];

			mean = (r + g + b) / 3;

			m_vHeightMap.push_back(mean);
		}
	}
	fin.close();

	if (m_vHeightMap.capacity() - m_vHeightMap.size() > static_cast<size_t>(0))
	{
		for (size_t i = m_vHeightMap.size(); i < m_vHeightMap.capacity(); i++)
		{
			m_vHeightMap.push_back(0);
		}
	}

	m_bHeightMap_Loaded = true;

	Safe_Delete_Array(pData);

	return S_OK;
}

HRESULT CTerrainBufferComp::Ready_Component()
{
	return E_NOTIMPL;
}

void CTerrainBufferComp::LateUpdate_Component()
{
}

void CTerrainBufferComp::Render_Component()
{
}

