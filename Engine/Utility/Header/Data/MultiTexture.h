#pragma once

#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CMultiTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CMultiTexture)

private:
	explicit CMultiTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMultiTexture();

public:
	static CMultiTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U)) override;
	virtual void	Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture);

protected:
	vector<LPDIRECT3DBASETEXTURE9>		m_vecMulti;			// 스테이트 키에 대한 텍스처 정보
	TEXTUREID				m_eType = TEX_NORMAL;			// 텍스처 종류
};

END