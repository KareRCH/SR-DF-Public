#include "SingleStateTexture.h"

CSingleStateTexture::CSingleStateTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
	m_eTexComType = ETEXTURE_COMTYPE::SINGLE_STATE;
}

CSingleStateTexture::~CSingleStateTexture()
{
}

CSingleStateTexture* CSingleStateTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(pGraphicDev)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSingleStateTexture::Free()
{
	for (auto item : m_mapTextureState)
	{
		Safe_Release(item.second);
	}
}

HRESULT CSingleStateTexture::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

	return S_OK;
}

HRESULT CSingleStateTexture::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	// 스테이트 키에 이미 텍스처 셋이 있으면 처리하지 않는다.
	if (m_mapTextureState.find(pStateKey) != m_mapTextureState.end())
		return E_FAIL;

	LPDIRECT3DBASETEXTURE9 pTexture = nullptr;

	TCHAR	szFileName[256] = L"";
	wsprintf(szFileName, pFilePath, L"");

	switch (eType)
	{
	case TEX_NORMAL:
	{
		FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture), E_FAIL);
		break;
	}
	case TEX_CUBE:
	{
		FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture), E_FAIL);
		break;
	}
	case TEX_VOLUME:
	{
		FAILED_CHECK_RETURN(D3DXCreateVolumeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DVOLUMETEXTURE9*)&pTexture), E_FAIL);
		break;
	}
	}

	m_mapTextureState.emplace(pStateKey, pTexture);

	return S_OK;
}

void CSingleStateTexture::Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, const _tchar* pStateKey)
{
	if (m_mapTextureState.empty())
		return;

	auto iter = m_mapTextureState.find(pStateKey);

	if (iter == m_mapTextureState.end())
		return;

	pVecTexture->clear();
	pVecTexture->push_back(m_mapTextureState[pStateKey]);
}
