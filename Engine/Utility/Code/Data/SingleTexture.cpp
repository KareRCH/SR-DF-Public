#include "SingleTexture.h"

CSingleTexture::CSingleTexture(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
	m_eTexComType = ETEXTURE_COMTYPE::SINGLE;
}

CSingleTexture::~CSingleTexture()
{
}

CSingleTexture* CSingleTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

void CSingleTexture::Free()
{
	Safe_Release(m_pTexture);
}

HRESULT CSingleTexture::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

    return S_OK;
}

HRESULT CSingleTexture::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
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

	m_pTexture = pTexture;

    return S_OK;
}

void CSingleTexture::Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture)
{
	if (m_pTexture == nullptr)
		return;

	pVecTexture->clear();
	pVecTexture->push_back(m_pTexture);
}
