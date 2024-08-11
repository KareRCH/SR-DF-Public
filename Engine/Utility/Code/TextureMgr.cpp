#include "TextureMgr.h"

#include "Export_System.h"

#include "SingleTexture.h"
#include "SingleStateTexture.h"
#include "MultiTexture.h"
#include "MultiStateTexture.h"

IMPLEMENT_SINGLETON(CTextureMgr)

CTextureMgr::CTextureMgr()
{
}

CTextureMgr::~CTextureMgr()
{
	Free();
}

void CTextureMgr::Free()
{
	for (auto item : m_mapTexture)
	{
		Safe_Release(item.second);
	}
}

HRESULT CTextureMgr::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

	return S_OK;
}

HRESULT CTextureMgr::Load_Texture(TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	

	return S_OK;
}

HRESULT CTextureMgr::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	CTexture* pTexture;
	HRESULT hr = S_OK;
	ETEXTURE_COMTYPE eComType;
	eComType = ETEXTURE_COMTYPE::MULTI_STATE;

	// ���� ���翩�� Ȯ��, �߸��� ��ΰ� �ִٸ� ���� ��ȯ
	//wifstream file(pFilePath);
	//FALSE_CHECK_RETURN(!file.good(), E_FAIL);

	// ����Ű�� �ؽ�ó ������ ���� �з�
	/*if (0U >= iCntRange.second - iCntRange.first)
	{
		if (pStateKey != L"")
			eComType = ETEXTURE_COMTYPE::SINGLE_STATE;
		else
			eComType = ETEXTURE_COMTYPE::SINGLE;
	}
	else
	{
		if (pStateKey != L"")
			eComType = ETEXTURE_COMTYPE::MULTI_STATE;
		else
			eComType = ETEXTURE_COMTYPE::MULTI;
	}*/


	// �ؽ�óŰ�� ���� �ؽ�ó�� �ִ��� ã�ƺ� �� ���ٸ� ���� �����.
	// �ƴϸ� ������ �ؽ�ó ��ü�� ������Ʈ Ű�� �߰��Ѵ�.

	// ���ؽ��� ���� m_mapTexture�� ��ü�� �����ϴ� �Ϳ� ���� ��ȣ�Ѵ�.
	{
		lock_guard<mutex> lock(m_mapMutex);			// �� �ڵ��ϸ� ��ȣ�Ѵ�.

		auto iter = m_mapTexture.find(pTextureKey);

		// �ؽ�ó�� ������ ���θ����.
		if (iter == m_mapTexture.end())
		{
			switch (eComType)
			{
			case ETEXTURE_COMTYPE::SINGLE:
				pTexture = CSingleTexture::Create(m_pGraphicDev);
				break;
			case ETEXTURE_COMTYPE::SINGLE_STATE:
				pTexture = CSingleStateTexture::Create(m_pGraphicDev);
				break;
			case ETEXTURE_COMTYPE::MULTI:
				pTexture = CMultiTexture::Create(m_pGraphicDev);
				break;
			case ETEXTURE_COMTYPE::MULTI_STATE:
				pTexture = CMultiStateTexture::Create(m_pGraphicDev);
				break;
			}
			m_mapTexture.emplace(pTextureKey, pTexture);
		}
		// �ؽ�ó�� ������ ������ �Ϳ� �߰��Ѵ�.
		else
		{
			// ������Ʈ Ű�� ���� ������ �ߺ��Ǹ� ó������ �ʴ´�.
			// ���� �ε�� �ؽ�ó �¸� �����Ѵ�.
			if (eComType == ETEXTURE_COMTYPE::SINGLE || eComType == ETEXTURE_COMTYPE::MULTI)
				return E_FAIL;

			pTexture = iter->second;
		}
	}

	FAILED_CHECK_RETURN(hr = pTexture->Insert_Texture(pFilePath, eType, pStateKey, iCntRange), E_FAIL);

	return hr;
}

HRESULT CTextureMgr::Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	auto iter = m_mapTexture.find(pTextureKey);
	FALSE_CHECK_RETURN(iter != m_mapTexture.end(), E_FAIL);		// Ű ������ ����

	CTexture* pTexture = iter->second;

	switch (pTexture->Get_TexComType())
	{
	case ETEXTURE_COMTYPE::SINGLE:
	{
		CSingleTexture* pSingleTex = dynamic_cast<CSingleTexture*>(pTexture);
		pSingleTex->Transfer_Texture(pVecTexture);
		break;
	}
	case ETEXTURE_COMTYPE::SINGLE_STATE:
	{
		CSingleStateTexture* pSingleTex = dynamic_cast<CSingleStateTexture*>(pTexture);
		pSingleTex->Transfer_Texture(pVecTexture, pStateKey);
		break;
	}
	case ETEXTURE_COMTYPE::MULTI:
	{
		CMultiTexture* pMultiTex = dynamic_cast<CMultiTexture*>(pTexture);
		pMultiTex->Transfer_Texture(pVecTexture);
		break;
	}
	case ETEXTURE_COMTYPE::MULTI_STATE:
	{
		CMultiStateTexture* pMultiTex = dynamic_cast<CMultiStateTexture*>(pTexture); //
		pMultiTex->Transfer_Texture(pVecTexture, pStateKey);
		break;
	}
	}

	return S_OK;
}
