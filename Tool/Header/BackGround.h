#pragma once

#include "GameObject.h"
#include "ImguiAnimationTool.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CBackGround : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBackGround)

private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround();

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	HRESULT				Add_Component();

private:
	// �ִϸ��̼� �ҷ�����
	void LoadAnimationFromFile(const char* fileName);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	void				KeyInput();

public:
	GETSET_EX2(CTransformComponent*,	m_pTransformComp,	TransformComponent,	GET, SET)
	GETSET_EX2(CRcBufferComp*,			m_pBufferComp,		BufferComponent,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pTextureComp,		TextureComponent,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pLeftHandTextureComp, LeftHandTextureComp,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pRightHandTextureComp, RightHandTextureComp, GET, SET)

private:
	CTransformComponent*			m_pTransformComp = nullptr;
	CRcBufferComp*					m_pBufferComp = nullptr;
	CTextureComponent*				m_pTextureComp = nullptr;
	CTextureComponent*				m_pLeftHandTextureComp = nullptr;
	CTextureComponent*				m_pRightHandTextureComp = nullptr;

	_matrix		m_ViewMatrix, m_ProjMatrix;

private: //����
	// dt ��
	_float fFrameTimeDelta, fCurFrameTimeDelta;

	// ũ��
	_float fSizeX_Delta, fSizeY_Delta;

	// ȸ��
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;

	// �̵�
	_float fPosX_Delta, fPosY_Delta;

	int TextureNum[KEYTYPE_END];

	// �ִϸ��̼� ��
	CImguiAnimationTool* m_pAnimationTool;

	// �ִϸ��̼� ��
	std::vector<KEYFRAME>*  m_vecAnimation[KEYTYPE_END];

	KEYTYPE* m_eCurType;

	//_bool bFrameOn = false;
};

