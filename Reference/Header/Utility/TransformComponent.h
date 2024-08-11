#pragma once

#include "SceneComponent.h"

BEGIN(Engine)

/// <summary>
/// Ʈ������ ������Ʈ�� ���ӿ�����Ʈ�� �߰��Ǿ� ������Ʈ�� ��ġ������ �����ϴ� ������Ʈ�̴�.
/// SceneComponent������ Ʈ�������� �����Ǳ�� ������ �̴� �� ������Ʈ�� ���� ��ǥ���� ������ ���̸�.
/// �� Ŭ�����ʹ� ����� ����.
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
	// ��ǥ �����Լ�
	void		Move_Pos(const _vec3* const pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}

	// ȸ�� ���� �Լ� (�� 1���� ����)
	void		Rotate(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vRotation) + eType) += fAngle;
	}

	// ���� ����
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