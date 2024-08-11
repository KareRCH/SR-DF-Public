#include "TransformComponent.h"

CTransformComponent::CTransformComponent()
{
    ZeroMemory(m_vInfo, sizeof(m_vInfo));
    D3DXMatrixIdentity(&m_matTransform);
}

CTransformComponent::CTransformComponent(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
    ZeroMemory(m_vInfo, sizeof(m_vInfo));
    D3DXMatrixIdentity(&m_matTransform);
}

CTransformComponent::CTransformComponent(const CTransformComponent& rhs)
    : Base(rhs)
{
    for (size_t i = 0; i < INFO_END; i++)
    {
        m_vInfo[i] = rhs.m_vInfo[i];
    }
	m_matTransform = rhs.m_matTransform;
}

CTransformComponent::~CTransformComponent()
{
}

CTransformComponent* CTransformComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Component()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Transform Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CTransformComponent::Clone()
{
	return new ThisClass(*this);
}

void CTransformComponent::Free()
{
	SUPER::Free();
}

HRESULT CTransformComponent::Ready_Component()
{
    D3DXMatrixIdentity(&m_matTransform);

    for (_int i = 0; i < INFO_END; ++i)
        memcpy(&m_vInfo[i], &m_matTransform.m[i][0], sizeof(_vec3));

    return S_OK;
}

_int CTransformComponent::Update_Component(const _float& fTimeDelta)
{
	/*D3DXMatrixIdentity(&m_matTransform);
	_matrix matTranslate, matRotation, matScale;

	D3DXMatrixTranslation(&matTranslate, m_vInfo[INFO_POS].x, m_vInfo[INFO_POS].y, m_vInfo[INFO_POS].z);
	D3DXMatrixRotationYawPitchRoll(&matRotation, m_vRotation.y, m_vRotation.x, m_vRotation.z);
	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	m_matTransform *= matScale * matRotation * matTranslate;*/
	D3DXMatrixIdentity(&m_matTransform);

	// 3x3만큼 월드 행렬로부터 vInfo에 복사
	for (_int i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matTransform.m[i][0], sizeof(_vec3));

	// 크기 변환
	for (_int i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	// 회전 변환
	_matrix		matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], m_vRotation.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vRotation.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vRotation.z);

	for (_int i = 0; i < INFO_POS; ++i)
	{
		for (_int j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	// 월드 행렬 구성
	for (_int i = 0; i < INFO_END; ++i)
		memcpy(&m_matTransform.m[i][0], &m_vInfo[i], sizeof(_vec3));

	return 0;
}

void CTransformComponent::Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed)
{
	_vec3	vDir = *pTargetPos - m_vInfo[INFO_POS];

	m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fTimeDelta * fSpeed;

	_matrix		matTrans;
	_matrix		matRot = *Compute_LootAtTarget(pTargetPos);

	D3DXMatrixTranslation(&matTrans,
		m_vInfo[INFO_POS].x,
		m_vInfo[INFO_POS].y,
		m_vInfo[INFO_POS].z);

	m_matTransform = matRot * matTrans;
}

const _matrix* CTransformComponent::Compute_LootAtTarget(const _vec3* pTargetPos)
{
	_vec3		vDir = *pTargetPos - m_vInfo[INFO_POS];

	_vec3		vAxis = *D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir);

	/*_matrix		matRot;
	_vec3		vUp;

	float		fDot = D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir), D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]));

	float		fAngle = acosf(fDot);

	D3DXMatrixRotationAxis(&matRot, &vAxis, fAngle);
	return &matRot;*/

	_matrix		matRot;
	_vec3		vUp;


	return D3DXMatrixRotationAxis(&matRot,
									&vAxis,
									acosf(D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir),
										D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]))));
}



