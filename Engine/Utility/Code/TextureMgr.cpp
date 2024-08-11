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

	// 파일 존재여부 확인, 잘못된 경로가 있다면 오류 반환
	//wifstream file(pFilePath);
	//FALSE_CHECK_RETURN(!file.good(), E_FAIL);

	// 상태키나 텍스처 개수에 따른 분류
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


	// 텍스처키로 먼저 텍스처가 있는지 찾아본 후 없다면 새로 만들고.
	// 아니면 기존의 텍스처 객체에 스테이트 키를 추가한다.

	// 뮤텍스를 통해 m_mapTexture에 객체를 제조하는 것에 대해 보호한다.
	{
		lock_guard<mutex> lock(m_mapMutex);			// 이 코드블록만 보호한다.

		auto iter = m_mapTexture.find(pTextureKey);

		// 텍스처가 없으니 새로만든다.
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
		// 텍스처가 있으니 기존에 것에 추가한다.
		else
		{
			// 스테이트 키가 없는 종류는 중복되면 처리하지 않는다.
			// 최초 로드된 텍스처 셋만 인정한다.
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
	FALSE_CHECK_RETURN(iter != m_mapTexture.end(), E_FAIL);		// 키 없으면 실패

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
