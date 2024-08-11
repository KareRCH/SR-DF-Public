#pragma once

#include "SceneComponent.h"

BEGIN(Engine)

/// <summary>
/// 트랜스폼 컴포넌트는 게임오브젝트에 추가되어 오브젝트의 위치정보를 저장하는 컴포넌트이다.
/// SceneComponent에서도 트랜스폼이 제공되기는 하지만 이는 그 컴포넌트를 위한 좌표값을 가지는 것이며.
/// 이 클래스와는 상관이 없다.
/// </summary>
class ENGINE_DLL CTransformComponent : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CTransformComponent)

protected:
	explicit CTransformComponent();
	explicit CTransformComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransformComponent(const CTransformComponent& rhs);
	virtual ~CTransformComponent();

public:
	static CTransformComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Component() override;
	virtual _int		Update_Component(const _float& fTimeDelta);
	virtual void		LateUpdate_Component() override {}
	virtual void		Render_Component() override {}

	void				Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed);
	const _matrix*		Compute_LootAtTarget(const _vec3* pTargetPos);

public:
	// 좌표 변경함수
	void		Move_Pos(const _vec3* const pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}

	// 회전 변경 함수 (축 1개만 가능)
	void		Rotate(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vRotation) + eType) += fAngle;
	}

	// 벡터 복사
	void			Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matTransform.m[eType][0], sizeof(_vec3));
	}

	_vec3		Get_Pos()	{ return m_vInfo[INFO_POS]; }
	_vec3		Get_Right() { return m_vInfo[INFO_RIGHT]; }
	_vec3		Get_Up()	{ return m_vInfo[INFO_UP]; }
	_vec3		Get_Look()	{ return m_vInfo[INFO_LOOK]; }

	void		Set_WorldMatrixS(const _matrix* pWorld) { m_matTransform = *pWorld; }

	void		Set_MovePos(const _float& fTimeDelta, const _vec3& fSpeed)
	{
		m_vInfo[INFO_POS] += { fSpeed.x* fTimeDelta,
			fSpeed.y* fTimeDelta,
			fSpeed.z* fTimeDelta };
	}
};

END