#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;
class CColliderComponent;

END

class CSolid abstract : public CGameObject
{
	DERIVED_CLASS(CGameObject, CSolid)

protected:
	explicit CSolid(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSolid(const CSolid& rhs);
	virtual ~CSolid();

protected: // 함수 
	virtual void		Free();											// Release

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	virtual void		OnCollision(CGameObject* pDst, const FContact* const pContact) PURE;
	virtual void		OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) PURE;
	virtual void		OnCollisionExited(CGameObject* pDst) PURE;

public:
	GETSET_EX2(CCubeBufferComp*, m_pCubeBufferComp, CubeBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)

protected:
	CCubeBufferComp* m_pCubeBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;

protected:
	virtual HRESULT		Add_Component();								// 컴포넌트 추가 
};

