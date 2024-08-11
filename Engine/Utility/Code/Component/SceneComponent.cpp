#include "SceneComponent.h"

CSceneComponent::CSceneComponent()
	: m_vRotation(0.f, 0.f, 0.f), m_vScale(1.f, 1.f, 1.f)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matTransform);
}

CSceneComponent::CSceneComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
	, m_vRotation(0.f, 0.f, 0.f), m_vScale(1.f, 1.f, 1.f)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matTransform);
}

CSceneComponent::CSceneComponent(const CSceneComponent& rhs)
	: Base(rhs)
	, m_vRotation(rhs.m_vRotation), m_vScale(rhs.m_vScale)
{
	for (size_t i = 0; i < INFO_END; i++)
	{
		m_vInfo[i] = rhs.m_vInfo[i];
	}
	m_matTransform = rhs.m_matTransform;
}

CSceneComponent::~CSceneComponent()
{
}

void CSceneComponent::Free()
{
	SUPER::Free();
}
