#pragma once
#include "PlayerAttackUnion.h"
#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CCloseAttack : public CPlayerAttackUnion
{
	DERIVED_CLASS(CPlayerAttackUnion, CCloseAttack)

private:
	explicit CCloseAttack(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCloseAttack(const CCloseAttack& rhs);
	virtual ~CCloseAttack();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CCloseAttack* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir,
		CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID,
		_float fMoveSpeed, _float fDeleteTime, _float fDamage, _float fSize,
		STATE_RIGHTHAND _pType);

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
		GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
		GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
		GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

		// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);

private:
	HRESULT				Add_Component();
	virtual void		Free();

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr; //플레이어용도 
	FCollisionSphere* pShape;
	//LPD3DXMESH	m_MeshSphere;

private:
	_bool* m_bDbugFrame;
	STATE_RIGHTHAND m_pWeaponType;
};

