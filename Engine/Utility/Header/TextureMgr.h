#pragma once

#include "Base.h"
#include "Engine_Define.h"

#include <mutex>

BEGIN(Engine)

class CTexture;

// 컴객체 텍스처의 타입
enum class ETEXTURE_COMTYPE : int
{
	SINGLE,
	SINGLE_STATE,
	MULTI,
	MULTI_STATE
};

/// <summary>
/// 텍스처를 관리하는 매니저
/// </summary>
class ENGINE_DLL CTextureMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CTextureMgr)

private:
	explicit CTextureMgr();
	virtual ~CTextureMgr();

public:
	virtual void	Free();

public:
	HRESULT			Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT			Load_Texture(TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange = _range<_uint>(0U, 0U));
	HRESULT			Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U));
	HRESULT			Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey = L"");

public:
	GETSET_EX1(mutex, m_mapMutex, Mutex, GET_PTR)

private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
	_unmap<wstring, CTexture*>		m_mapTexture;
	mutex							m_mapMutex;				// 텍스처 객체 제조시 동기화

};

END