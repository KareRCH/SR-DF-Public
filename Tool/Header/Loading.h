#pragma once

#include "Base.h"
#include "Engine_Define.h"

#include <future>


class CLoading : public CBase
{
	DERIVED_CLASS(CBase, CLoading)
public:
	enum LOADINGID {LOADING_TEXTURE, LOADING_STAGE, LOADING_BOSS, LOADING_END };

private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	static CLoading*	Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);

private:
	virtual void		Free();

public:
	LOADINGID			Get_LoadingID()const	{ return m_eLoadingID; }
	CRITICAL_SECTION*	Get_Crt()				{ return &m_Crt; }
	_bool				Get_Finish()const		{ return m_bFinish; }

public:
	HRESULT				Ready_Loading(LOADINGID eID);
	_uint				Loading_For_Stage();
	HRESULT				Loading_For_Texture();
	// 비동기 로드 함수
	void				Load_TextureAsync(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange = _range<_uint>(0U, 0U));
	void				Load_Texture(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange = _range<_uint>(0U, 0U), _bool bAsync = true);
	void				Wait_LoadTextureAsync();

public:
	static _uint	CALLBACK Thread_Main(void* pArg);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_bool					m_bFinish;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_Crt;
	LOADINGID				m_eLoadingID;

protected:
	virtual HRESULT		Ready_Layer_Completed() { return S_OK; }

protected:
	vector<future<void>>	m_listAsyncTexture;

};

