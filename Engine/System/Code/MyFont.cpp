#include "MyFont.h"

CMyFont::CMyFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pFont(nullptr)
	, m_pSprite(nullptr)
{
	m_pGraphicDev->AddRef();
}

CMyFont::~CMyFont()
{
}

HRESULT CMyFont::Ready_Font(const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
	{
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}

	D3DXFONT_DESC		Font_Desc;
	ZeroMemory(&Font_Desc, sizeof(D3DXFONT_DESC));

	Font_Desc.CharSet = HANGEUL_CHARSET;
	Font_Desc.Width = iWidth;
	Font_Desc.Height = iHeight;
	Font_Desc.Weight = iWeight;
	lstrcpy(Font_Desc.FaceName, pFontType);

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &Font_Desc, &m_pFont)))
	{
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CMyFont::Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	RECT	rc{ LONG(pPos->x), LONG(pPos->y) };

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_NOCLIP, Color);

	m_pSprite->End();
}

CMyFont* CMyFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		Safe_Release(pInstance);
		MSG_BOX("폰트 문제야");
		return nullptr;
	}

	return pInstance;
}

void CMyFont::Free()
{
}
