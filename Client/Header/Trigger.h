#pragma once

#include "GameObject.h"
#include "UI_FadeIn.h"
#include "UI_FadeOut.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"


BEGIN(Engine)

class CTransformComponent;
class CColliderComponent;

END

class CTrigger : public CGameObject
{
	DERIVED_CLASS(CGameObject, CTrigger)

protected:
	explicit CTrigger(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrigger(const CTrigger& rhs);
	virtual ~CTrigger();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() PURE;
	virtual void		Free() override;

	// 성희 추가 : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(_float _Width, _float _Height, _float Depth) override;
	virtual void		MeshSphereColider(_float Radius, _uint Slices, _uint Stacks) override;

public:
	virtual void		OnCollision(CGameObject* pDst, const FContact* const pContact) PURE;
	virtual void		OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) PURE;
	virtual void		OnCollisionExited(CGameObject* pDst) PURE;

protected:
	virtual HRESULT		Add_Component();

public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)


protected:
	CTransformComponent* m_pTransformComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;

	FBlackBoardPtr<CBlackBoard_Player>	m_wpBlackBoard_Player;	// 블랙보드 플레이어

protected:
	_bool	m_bIsTriggered = false;
	_bool	FadeInEnd = false;
};

