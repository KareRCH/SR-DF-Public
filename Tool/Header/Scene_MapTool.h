#pragma once

#include "Scene.h"
#include <future>

class CLoading;

/// <summary>
/// 맵툴 전용 씬
/// </summary>
class CScene_MapTool : public Engine::CScene
{
	DERIVED_CLASS(CScene, CScene_MapTool)
private:
	explicit CScene_MapTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_MapTool();

public:
	static CScene_MapTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

protected:
	HRESULT				Ready_Texture();
	HRESULT				Ready_Proto();
	virtual HRESULT		Ready_Layer_Completed();

	void Load_TextureAsync(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange = _range<_uint>(0U, 0U));
	void Load_Texture(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange = _range<_uint>(0U, 0U), _bool bAsync = true);
	void Wait_LoadTextureAsync();

protected:
	list<future<void>>	m_listAsyncTexture;
	_uint				m_iFutureLimit;			// 비동기 개수 제한

private:
	DELAY<float>		m_fSkipStartFrame = DELAY<float>(0.1f);
	_bool				m_bInitFrame;
	CLoading*			m_pLoading;
	HRESULT				m_hrLoading;

};

