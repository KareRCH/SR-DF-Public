#pragma once
#include "Texture.h"

BEGIN(Engine)

/// <summary>
/// ���¸� ������ �ؽ�ó ����
/// ������Ʈ Ű�� ���� 
/// </summary>
class CMultiStateTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CMultiStateTexture)

private:
	explicit CMultiStateTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMultiStateTexture();

public:
	static CMultiStateTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U)) override;
	virtual HRESULT Insert_TextureAsync(const _tchar* pFilePath, TEXTUREID eType, vector<LPDIRECT3DBASETEXTURE9>& vecTexture, _uint iIndex);
	virtual void	Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, const _tchar* pStateKey);

protected:
	using multi_tex_map = _unmap<wstring, vector<LPDIRECT3DBASETEXTURE9>>;
	multi_tex_map			m_mapMultiState;				// ������Ʈ Ű�� ���� �ؽ�ó ����
	TEXTUREID				m_eType = TEX_NORMAL;			// �ؽ�ó ����

	
};

END