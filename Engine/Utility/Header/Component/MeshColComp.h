#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CMeshColComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CMeshColComp)

protected:
	explicit CMeshColComp();
	explicit CMeshColComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMeshColComp(const CMeshColComp& rhs);
	virtual ~CMeshColComp();

public:
	void	BoxMesh_Ready(LPDIRECT3DDEVICE9 pDevice, FLOAT _Width, FLOAT _Height, FLOAT Depth);
	void	Sphere_Ready(LPDIRECT3DDEVICE9  pDevice, FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXBUFFER* ppAdjacency);
	void	BoxMesh_Col();
	void	SphereMesh_Col();

public:
	static CMeshColComp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	LPD3DXMESH	m_MeshBox;
	LPD3DXMESH	m_MeshSphere;

	FLOAT  Width = 0.f;
	FLOAT  Height = 0.f;
	FLOAT  Depth = 0.f;

private:
	virtual void Free();

	// CVIBufferComp을(를) 통해 상속됨
	virtual HRESULT Ready_Component() override;
	virtual void LateUpdate_Component() override;
	virtual void Render_Component() override;
};

END