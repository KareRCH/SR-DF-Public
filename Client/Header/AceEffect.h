#pragma once
#include "AceGameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Define.h"

#include "BlackBoard_Monster.h"
#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CAceEffect : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CAceEffect)

protected:
	explicit CAceEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceEffect(const CAceEffect& rhs);
	virtual ~CAceEffect();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free();

protected:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;

public:
	HRESULT				Billboard();
	HRESULT				Add_Component();
	void				Height_On_Terrain(_float Height);
	void				Owner_Dead(CGameObject* pOwner);
	
	// 블랙보드
protected:
	FBlackBoardPtr<CBlackBoard_Monster>	m_wpBlackBoard_Monster;	
	FBlackBoardPtr<CBlackBoard_Player>	m_wpBlackBoard_Player;
	FRAME				m_tFrame;
	_bool						m_bOwnerDead;

public:
	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

	// Get_RandomFloat의 vector형 
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}
};

