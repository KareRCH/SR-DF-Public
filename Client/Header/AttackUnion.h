#pragma once
#include "AceUnit.h"

class CAttackUnion : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAttackUnion)

protected:
	explicit CAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAttackUnion(const CAttackUnion& rhs);
	virtual ~CAttackUnion();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

	// ¼ºÈñ Ãß°¡ : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(_float _Width, _float _Height, _float Depth) override;
	virtual void		MeshSphereColider(_float Radius, _uint Slices, _uint Stacks) override;

};