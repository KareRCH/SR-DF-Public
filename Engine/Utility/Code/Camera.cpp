#include "Camera.h"

#include "Export_System.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev),
	m_vEye(), m_vAt(), m_vUp(),
	m_fFov(), m_fAspect(), m_fNear(), m_fFar(),
	m_fWidth(100.f), m_fHeight(100.f)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::~CCamera()
{
}

void CCamera::Free()
{
	SUPER::Free();
}

HRESULT CCamera::Ready_GameObject()
{
	// �� ����� ����� ��ġ���� �Ѱ� ���´�.
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// ���� ��ȯ ����� �ѹ� ������ָ� �ٽ� �ȸ���� �־ �ȴ�.
	// �ٸ� �ɼ��� �ٲ�� ����� �־�� �Ѵ�.
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
	// �� ����� ī�޶� �� �����Ӹ��� �����̱� ������ �Ź� ����� �־���Ѵ�.
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	/*if (Engine::Get_DIKeyState(DIK_UP) & 0x80)
	{
		m_fHeight++;
	}
	else if (Engine::Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		
		m_fHeight--;
		if (m_fHeight < 10.f)
			m_fHeight = 10.f;
	}

	if (Engine::Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		m_fWidth++;
			
	}
	else if (Engine::Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		m_fWidth--;
		if (m_fWidth < 10.f)
			m_fWidth = 10.f;
	}*/

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return 0;
}