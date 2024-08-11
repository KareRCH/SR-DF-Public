#pragma once

#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CSingleStateTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CSingleStateTexture)

private:
	explicit CSingleStateTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSingleStateTexture();

public:
	static CSingleStateTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U)) override;
	virtual void	Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, const _tchar* pStateKey);

protected:
	using multi_tex_map = _unmap<wstring, LPDIRECT3DBASETEXTURE9>;
	multi_tex_map			m_mapTextureState;				// 스테이트 키에 대한 텍스처 정보
	TEXTUREID				m_eType = TEX_NORMAL;			// 텍스처 종류
	
};

END