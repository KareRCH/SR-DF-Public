#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "TextureMgr.h"

BEGIN(Engine)

/// <summary>
/// 텍스처 데이터 저장용 클래스
/// 추상클래스
/// </summary>
class ENGINE_DLL CTexture abstract : public CBase
{
	DERIVED_CLASS(CBase, CTexture)

protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTexture();

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free() PURE;

public:
	virtual HRESULT		Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev) PURE;
	virtual HRESULT		Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U)) PURE;
	
protected:
	virtual void		Transfer_Texture() {}

public:
	GETSET_EX1(ETEXTURE_COMTYPE, m_eTexComType, TexComType, GET)

protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;						// 장치
	ETEXTURE_COMTYPE			m_eTexComType = ETEXTURE_COMTYPE::SINGLE;		// 멀티텍스쳐인지 확인하는 용도

};

END