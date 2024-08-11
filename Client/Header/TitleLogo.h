#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CTitleLogo : public CGameObject
{
	DERIVED_CLASS(CGameObject, CTitleLogo)

private:
	explicit CTitleLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTitleLogo(const CTitleLogo& rhs);
	virtual ~CTitleLogo();

public:
	static CTitleLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	HRESULT				Add_Component();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
};

