#pragma once
#include "AceGameObject.h"

/// <summary>
/// Player, Monster, Attack�� ��� �޾Ƽ� ��� 
/// </summary>
class CAceUnit : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CAceUnit)

protected:
	explicit CAceUnit(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceUnit(const CAceUnit& rhs);
	virtual ~CAceUnit();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

	// ���� �߰� : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(_float _Width, _float _Height, _float Depth) override;
	virtual void		MeshSphereColider(_float Radius, _uint Slices, _uint Stacks) override;

protected:
	GAUGE<_float>		m_gHp;

protected: // ����
	_bool m_IsOnTerrain = false;
	_bool m_IsOnGround = false;

	_vec3 m_vSpeed = { 0.f, 0.f, 0.f };
	_vec3 m_vAccel = { 0.f, -27.8f, 0.f };

protected:
	void Gravity(const _float& fTimeDelta);

};