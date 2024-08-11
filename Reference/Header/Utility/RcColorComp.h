#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CRcColorComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CRcColorComp)

protected:
	explicit CRcColorComp();
	explicit CRcColorComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcColorComp(const CRcColorComp& rhs);
	virtual ~CRcColorComp();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CRcColorComp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();

	// CVIBufferComp을(를) 통해 상속됨
	virtual HRESULT Ready_Component() override;
	virtual void LateUpdate_Component() override;
	virtual void Render_Component() override;
};

END