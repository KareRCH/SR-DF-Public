#pragma once

#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CSingleTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CSingleTexture)

private:
	explicit CSingleTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSingleTexture();

public:
	static CSingleTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U)) override;
	virtual void	Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture);

protected:
	LPDIRECT3DBASETEXTURE9		m_pTexture = nullptr;			// 텍스처 베이스
	TEXTUREID					m_eType = TEX_NORMAL;			// 텍스처 종류



	
};

END