#pragma once

#include "GameObject.h"

#include "AceObjectFactory.h"

BEGIN(Engine)

class CRcBufferComp;
class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CBat : public CGameObject
{
	DERIVED_CLASS(CGameObject, CBat)

private:
	explicit CBat(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBat(const CBat& rhs);
	virtual ~CBat();

public:
	static CBat* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 vPos, const _vec3 vRot, const _vec3 vScale);

private: // 함수 
	virtual void		Free();											// Release

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const _vec3 vPos, const _vec3 vRot, const _vec3 vScale);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	virtual void		OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void		OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void		OnCollisionExited(CGameObject* pDst) override;

public:
	GETSET_EX2(CCubeBufferComp*, m_pCubeBufferComp, CubeBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

private:
	CCubeBufferComp* m_pCubeBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;


private:
	HRESULT				Add_Component();								// 컴포넌트 추가 

};

