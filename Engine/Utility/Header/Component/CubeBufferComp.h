#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CCubeBufferComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CCubeBufferComp)
private:
	explicit CCubeBufferComp();
	explicit CCubeBufferComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeBufferComp(const CCubeBufferComp& rhs);
	virtual ~CCubeBufferComp();

public:
	virtual HRESULT		Ready_Component() override;
	virtual void		LateUpdate_Component() override;
	virtual void		Render_Component() override;

public:
	static CCubeBufferComp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();

public:
	const _vec3* const Get_VtxPos() const { return m_pVertexPos; }

private:
	_vec3* m_pVertexPos;
	
};

END