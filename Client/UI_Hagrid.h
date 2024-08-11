#pragma once
#include "UI.h"

/// <summary>
/// Çªµå ¸Ô¾úÀ»¶§ È­¸é ³ì»öµÇ´Â°Å
/// </summary>
class CUI_Hagrid : public CUI
{
	DERIVED_CLASS(CUI, CUI_Hagrid)

protected:
	explicit CUI_Hagrid(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_Hagrid(const CUI_Hagrid& rhs);
	virtual ~CUI_Hagrid();

public:
	static CUI_Hagrid* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	//---------------------------------------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
		GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
		GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)

		//---------------------------------------------------------------------------------------------
private:
	void				Update_InternalData();
	HRESULT				Add_Component();
	FRAME				m_tFrame;
	_float m_fCX;
	_bool _bLeft = false;
	_bool _bRight = false;
};
