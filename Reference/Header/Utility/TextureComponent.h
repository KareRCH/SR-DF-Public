#pragma once

#include "SceneComponent.h"



BEGIN(Engine)

class ENGINE_DLL CTextureComponent : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CTextureComponent)

private:
	explicit CTextureComponent();
	explicit CTextureComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTextureComponent(const CTextureComponent& rhs);
	virtual ~CTextureComponent();

public:
	static CTextureComponent*	Create(LPDIRECT3DDEVICE9 pGraphicDev, 
											TEXTUREID eID, 
											const _tchar* pTextureKey, const _tchar* pStateKey = L"");
	virtual CComponent*			Clone();

private:
	virtual void				Free();

public:
	PRIVATE virtual		HRESULT Ready_Component() override { return S_OK; }
	PUBLIC	virtual		HRESULT Ready_Component(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey = L"");
	PUBLIC	virtual		_int	Update_Component(const _float& fTimeDelta) override;
	PUBLIC	virtual		void	LateUpdate_Component() override;
	PRIVATE virtual		void	Render_Component() override {}
	PUBLIC	virtual		void	Render_Component(const _uint& iIndex = 0, _bool bLocalTransform = false);

public:
	void		Render_Texture(const _uint& iIndex = 0, _bool bLocalTransform = false);
	HRESULT		Receive_Texture(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey = L"");

public:
	GETSET_EX2(vector<LPDIRECT3DBASETEXTURE9>, m_vecTexture, VecTexture, GET_PTR, SET_PTR)

private:
	vector<LPDIRECT3DBASETEXTURE9>		m_vecTexture;

};

END