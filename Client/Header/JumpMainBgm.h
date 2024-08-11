#pragma once

#include "Trigger.h"

#include "Serialize_BaseClass.h"

class CJumpMainBgm : public CTrigger
{
	DERIVED_CLASS(CTrigger, CJumpMainBgm)

protected:
	explicit CJumpMainBgm(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CJumpMainBgm(const CJumpMainBgm& rhs);
	virtual ~CJumpMainBgm();

public:
	static CJumpMainBgm* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

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
	void Update_InternalData();
	_bool m_bSoundOff = false;
};

