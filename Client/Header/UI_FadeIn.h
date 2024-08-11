#pragma once
#include "UI.h"
#include "Trigger.h"

/// <summary>
/// ���̵��ο� 
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

// ���̵��� : ����ȭ�鿡�� ������ ������� ��Ÿ���°� 
// ���̵� �ƿ� : ������ ��ο����ٰ� �ȭ���� �Ǵ°� 
