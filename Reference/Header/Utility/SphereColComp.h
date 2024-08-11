#pragma once

#include "ColliderComponent.h"

BEGIN(Engine)

class ENGINE_DLL CSphereColComp : public CColliderComponent
{
	DERIVED_CLASS(CColliderComponent, CSphereColComp)

protected:
	explicit CSphereColComp();
	explicit CSphereColComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSphereColComp(const CSphereColComp& rhs);
	virtual ~CSphereColComp();

public:
	static CSphereColComp*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
	virtual void		Free();

public:
	virtual HRESULT Ready_Collider() override;
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}
	
};

END