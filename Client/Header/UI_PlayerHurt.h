#pragma once
#include "UI.h"

/// <summary>
/// Çªµå ¸Ô¾úÀ»¶§ È­¸é ³ì»öµÇ´Â°Å
/// </summary>
class CUI_PlayerHurt : public CUI
{
	DERIVED_CLASS(CUI, CUI_PlayerHurt)

protected:
	explicit CUI_PlayerHurt(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_PlayerHurt(const CUI_PlayerHurt& rhs);
	virtual ~CUI_PlayerHurt();

public:
	static CUI_PlayerHurt* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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

};
