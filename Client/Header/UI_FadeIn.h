#pragma once
#include "UI.h"
#include "Trigger.h"

/// <summary>
/// 페이드인용 
/// </summary>
class CUI_FadeIn : public CUI
{
	DERIVED_CLASS(CUI, CUI_FadeIn)

protected:
	explicit CUI_FadeIn(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_FadeIn(const CUI_FadeIn& rhs);
	virtual ~CUI_FadeIn();

public:
	static CUI_FadeIn* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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

	_bool m_bEnd = false;

};

// 페이드인 : 검은화면에서 서서히 밝아지며 나타나는것 
// 페이드 아웃 : 서서히 어두워지다가 까만화면이 되는것 
