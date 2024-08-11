#pragma once

#include "UI.h"

/// <summary>
/// �����ϴ� ����ڽ� ���� ǥ���� �÷��̾� ���̹��� ���������ϴ� UI
/// </summary>

class CUI_PlayerFace : public CUI
{
	DERIVED_CLASS(CUI, CUI_PlayerFace)
protected:
	explicit CUI_PlayerFace(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_PlayerFace(const CUI_PlayerFace& rhs);
	virtual ~CUI_PlayerFace();

public:
	static CUI_PlayerFace* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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
	void				Change_Texture(const _float& fTimeDelta);


private:
	GAUGE<_float>		m_gPlayerHP;
	_bool				m_bPlayerHit;
	_bool				m_bTrigger;
	_bool				m_bFaceTurn;

	_bool Random_variable(_int _winning)
	{
		//�ü�� �ܿ��� �����ϴ� ��¥ ���� - ��ǻ�Ͱ� �ֺ��� ȯ��� ������������ ��ȣ�ۿ��ϸ鼭 ����°�
		// �ǻ糭������ ������ �����ϴ� �ӵ��� �ſ� ������. 
		random_device rd;

		// �޸��� Ʈ�����Ͷ�� �˰����� ����ؼ� ���� ���� ������ �ʱ�ȭ
		mt19937 gen(rd());

		// 0 ���� 99 ���� ����Ȯ������ ����   (min, max)
		uniform_int_distribution<_int> iDistribution(0, 99);

		//������ ���� 
		_int RandomValue = iDistribution(gen);

		//�Ű������� 5�ϰ�� 5���� Ȯ���� true�� ��ȯ�ȴ�
		return (RandomValue <= _winning);
	}
};

// Healthy(100 ~ 80) - SlightlyHurt (80 ~ 60) - Hurt (60~40) - Moderate Hurt (40~20) - BadlyHurt(20 ~ 0)  
// Healthy  SlightlyHurt  Hurt  ModeratelyHurt  BadlyHurt
// ��� - normal - left - normal - right ���� 
// ���� ��ġ�� ���� left pain ��� 

// �����Ӽ���
// 0 ����, 1����, 2������, 4 ���̵忡�� ��Ʈ 

// TRI FACE
//	T	T	
//	T	F	= �߾� �ٶ󺸰� ����
//	F	T	= ó������ ���ư� - Ʈ���� X���� ��/�� ǥ������ 
//	F	F