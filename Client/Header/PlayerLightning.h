#pragma once
#include "PlayerSkillUnion.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "AceMonster.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CPlayerLightning : public CPlayerSkillUnion
{
	DERIVED_CLASS(CPlayerSkillUnion, CPlayerLightning)

private:
	explicit CPlayerLightning(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerLightning(const CPlayerLightning& rhs);
	virtual ~CPlayerLightning();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CPlayerLightning* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _pGameObject);
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
		GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
		GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
		GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

		//GETSET_EX2(_vec3, m_tBullet.vDir, AttackDir, GET, SET)

		// Ãæµ¹ -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);

private:
	HRESULT				Add_Component();
	virtual void		Free();

private:
	_bool* m_bDbugFrame;
	_bool  m_bSoundOff = false;
	_float m_fVolume = 0.5f;
	FCollisionSphere* pShape;
	_vec3	m_vPos = { 0.f, 0.f, 0.f };

};

