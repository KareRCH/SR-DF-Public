#pragma once

#include "GameObject.h"

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

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	virtual void OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionExited(CGameObject* pDst) override;

public:
	GETSET_EX2(CTransformComponent*,	m_pTransformComp,	TransformComponent,	GET, SET)
	GETSET_EX2(CRcBufferComp*,			m_pBufferComp,		BufferComponent,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pTextureComp,		TextureComponent,	GET, SET)

private:
	CTransformComponent*			m_pTransformComp = nullptr;
	CRcBufferComp*					m_pBufferComp = nullptr;
	CTextureComponent*				m_pTextureComp = nullptr;
	CTextureComponent*				m_pBackTextureComp = nullptr;

	_matrix		m_ViewMatrix, m_ProjMatrix;
private:
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

};

