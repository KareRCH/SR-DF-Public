#include "MeshColComp.h"

CMeshColComp::CMeshColComp()
{
}

CMeshColComp::CMeshColComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CMeshColComp::CMeshColComp(const CMeshColComp& rhs)
	: Base(rhs)
{
}

CMeshColComp::~CMeshColComp()
{
}

void CMeshColComp::BoxMesh_Ready(LPDIRECT3DDEVICE9 pDevice, FLOAT _Width, FLOAT _Height, FLOAT Depth)
{
	D3DXCreateBox(pDevice, Width, Height, Depth, &m_MeshBox, NULL);
}

void CMeshColComp::Sphere_Ready(LPDIRECT3DDEVICE9  pDevice, FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXBUFFER* ppAdjacency)
{
	D3DXCreateSphere(pDevice, Radius, Slices, Stacks, &m_MeshSphere, ppAdjacency);
}

void CMeshColComp::BoxMesh_Col()
{
	m_MeshBox->DrawSubset(0);
}

void CMeshColComp::SphereMesh_Col()
{
	m_MeshSphere->DrawSubset(0);
}

CMeshColComp* CMeshColComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMeshColComp* pInstance = new CMeshColComp(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MeshCol Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CMeshColComp::Clone()
{
	// 복사 생성
	return new CMeshColComp(*this);
}

void CMeshColComp::Free()
{
	SUPER::Free();
}

HRESULT CMeshColComp::Ready_Component()
{
	return E_NOTIMPL;
}

void CMeshColComp::LateUpdate_Component()
{
}

void CMeshColComp::Render_Component()
{
}
