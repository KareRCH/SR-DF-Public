#pragma once
#include "UI.h"

/// <summary>
///  ���̵�ƿ��� 
/// 
/// </summary>
class CUI_FadeOut : public CUI
{
	DERIVED_CLASS(CUI, CUI_FadeOut)

protected:
	explicit CUI_FadeOut(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_FadeOut(const CUI_FadeOut& rhs);
	virtual ~CUI_FadeOut();

public:
	static CUI_FadeOut* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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
	void				Update_BlackBoard();
	HRESULT				Add_Component();
	FRAME				m_tFrame;
	_bool				m_bEnd = false;

};

// ���̵��� : ����ȭ�鿡�� ������ ������� ��Ÿ���°� 
// ���̵� �ƿ� : ������ ��ο����ٰ� �ȭ���� �Ǵ°� 
/*
 "Stage1");
v, "BossStage");
v, "Malone");
v, "SeongHee");
*/