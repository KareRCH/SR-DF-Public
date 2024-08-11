#pragma once

#include "UI.h"

/// <summary>
/// ȭ�� �߾� ũ�ν����� 
/// </summary>
class CUI_CrossHair : public CUI
{
	DERIVED_CLASS(CUI, CUI_CrossHair)

protected:
	explicit CUI_CrossHair(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_CrossHair(const CUI_CrossHair& rhs);
	virtual ~CUI_CrossHair();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;
	virtual void Render_GameObject() override;

	static CUI_CrossHair* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	HRESULT Add_Component();
	virtual void Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	void				Update_InternalData();
	void				Change_Texture();

public:
	_bool				m_bPlayerEquipGunState;	// �÷��̾ �������������
	_bool				m_bPlayerAttackState;	// �÷��̾ �����Ҷ� (�ָ�, ���, ������ ��)
	_bool				m_bPlayerPickUpState;	// ��� ��������� - ��ü 
	_bool				m_bPlayerDetect;		// �÷��̾� ���������� ���������� 

	_bool HairSpin;
};

// ���� �ǰ� - ����
// �� ���ݻ�Ÿ� - �Ϲ� + ���ٰ� ������ x
// ������ - �ȸ¾Ƶ� �⺻ ������� ������ ��3�� ȸ��  
// ���� - �ȸ¾Ƶ� �⺻ ������� ������ ��3�� ȸ�� 
// ������ �غ� - ����
// ������ �¾Ƶ� �� �״�� 