#pragma once

#include "Trigger.h"

#include "Serialize_BaseClass.h"

class CTrigger_ToStageHall : public CTrigger
{
	DERIVED_CLASS(CTrigger, CTrigger_ToStageHall)

protected:
	explicit CTrigger_ToStageHall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrigger_ToStageHall(const CTrigger_ToStageHall& rhs);
	virtual ~CTrigger_ToStageHall();

public:
	static CTrigger_ToStageHall* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() {}
	virtual void		Free() override;

	// ���� �߰� : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(_float _Width, _float _Height, _float Depth) override;
	virtual void		MeshSphereColider(_float Radius, _uint Slices, _uint Stacks) override;

public:
	virtual void		OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void		OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void		OnCollisionExited(CGameObject* pDst) override;

protected:
	virtual HRESULT		Add_Component();

private:
	void	Update_InternalData();
};

