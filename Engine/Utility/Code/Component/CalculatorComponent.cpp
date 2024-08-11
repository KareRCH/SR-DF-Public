#include "CalculatorComponent.h"
#include "Export_System.h"
#include "Export_Utility.h"

CCalculatorComponent::CCalculatorComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CCalculatorComponent::~CCalculatorComponent()
{
}

HRESULT CCalculatorComponent::Ready_Calculator()
{
	return S_OK;
}

CCalculatorComponent* CCalculatorComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Calculator Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CCalculatorComponent::Clone()
{
	return new ThisClass(*this);
}

void CCalculatorComponent::Free()
{
	SUPER::Free();
}

_float CCalculatorComponent::Compute_HeightOnTerrain(
	const _vec3* pPos, const _vec3* pTerrainVtxPos,
	const _ulong& dwCntX, const _ulong& dwCntZ,
	const _vec3& vScale, const _vec3& vInvOffet)
{
	// 지형의 높이를 계산하는 함수
	_ulong	dwIndex = _ulong((pPos->z + vInvOffet.z) / vScale.z) * dwCntX 
								+ _ulong((pPos->x + vInvOffet.x) / vScale.x);

	_float	fRatioX = _float(pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / vScale.x;
	_float	fRatioZ = _float(pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / vScale.z;

	D3DXPLANE	Plane;

	// 오른쪽 상단 삼각형
	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	// 왼쪽 하단 삼각형
	else
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	// ax + by + cz + d
	// by = -ax - cz - d
	// y = (-ax - cz - d) / b

	// 평면의 방정식으로 y의 값을 반환한다.
	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
}

_vec3 CCalculatorComponent::Picking_On_Object_Local(HWND hWnd,
	CRcBufferComp* pRcBufferComp,
	CTransformComponent* pTransformComp)
{
	POINT	ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);


	// 뷰포트 변환 -> 투영 변환
	D3DXVECTOR3	vMouse;
	vMouse.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMouse.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMouse.z = 0.f;

	// 투영 -> 뷰 스페이스
	_matrix		matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMouse, &vMouse, &matProj);

	// 뷰 스페이스 -> 월드
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3		vRayPos, vRayDir;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMouse - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드 -> 로컬
	_matrix matWorld = *(pTransformComp->Get_Transform());
	D3DXMatrixInverse(&matWorld, 0, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	const _vec3* pRectVtxPos = pRcBufferComp->Get_VtxPos();

	_ulong	dwVtxIdx[3]{};

	_float fU(0.f), fV(0.f), fDist(0.f);

	_long dwCntX = 2;
	_long dwCntZ = 2;

	for (_ulong i = 0; i < dwCntZ - 1UL; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1UL; ++j)
		{
			_ulong	dwIndex = i * dwCntX + j;

			// 오른쪽 위

			dwVtxIdx[0] = dwIndex + dwCntX;
			dwVtxIdx[1] = dwIndex + dwCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pRectVtxPos[dwVtxIdx[1]],
				&pRectVtxPos[dwVtxIdx[0]],
				&pRectVtxPos[dwVtxIdx[2]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				//V1 + U(V2 - V1) + V(V3 - V1)
				_vec3 A = pRectVtxPos[dwVtxIdx[1]] + fU * (pRectVtxPos[dwVtxIdx[0]] - pRectVtxPos[dwVtxIdx[1]])
					+ fV * (pRectVtxPos[dwVtxIdx[2]] - pRectVtxPos[dwVtxIdx[1]]);

				return A;
			}

			// V1 + U(V2 - V1) + V(V3 - V1) 

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + dwCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pRectVtxPos[dwVtxIdx[2]],
				&pRectVtxPos[dwVtxIdx[1]],
				&pRectVtxPos[dwVtxIdx[0]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				return _vec3(pRectVtxPos[dwVtxIdx[2]].x + (pRectVtxPos[dwVtxIdx[1]].x - pRectVtxPos[dwVtxIdx[2]].x) * fU,
					0.f,
					pRectVtxPos[dwVtxIdx[2]].z + (pRectVtxPos[dwVtxIdx[0]].z - pRectVtxPos[dwVtxIdx[2]].z) * fV);
			}
		}
	}

	return _vec3{ 0,0,0 };
}

_vec3 CCalculatorComponent::Picking_On_Object_World(HWND hWnd, CRcBufferComp* pRcBufferComp, CTransformComponent* pTransformComp)
{

	POINT	ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(hWnd, &ptMouse);

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);


	// 뷰포트 변환 -> 투영 변환
	D3DXVECTOR3	vMouse;
	vMouse.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMouse.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMouse.z = 0.f;

	// 투영 -> 뷰 스페이스
	_matrix		matProj;

	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMouse, &vMouse, &matProj);

	// 뷰 스페이스 -> 월드
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3		vRayPos, vRayDir;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMouse - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	const _vec3* pRectVtxPos = pRcBufferComp->Get_VtxPos();

	_ulong	dwVtxIdx[3]{};

	_float fU(0.f), fV(0.f), fDist(0.f);

	int dwCntX = 2;
	int dwCntZ = 2;

	for (_ulong i = 0; i < dwCntZ - 1UL; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1UL; ++j)
		{
			_ulong	dwIndex = i * dwCntX + j;

			// 오른쪽 위

			dwVtxIdx[0] = dwIndex + dwCntX;
			dwVtxIdx[1] = dwIndex + dwCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pRectVtxPos[dwVtxIdx[1]],
				&pRectVtxPos[dwVtxIdx[0]],
				&pRectVtxPos[dwVtxIdx[2]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				//V1 + U(V2 - V1) + V(V3 - V1)
				_vec3 A = pRectVtxPos[dwVtxIdx[1]] + fU * (pRectVtxPos[dwVtxIdx[0]] - pRectVtxPos[dwVtxIdx[1]])
					+ fV * (pRectVtxPos[dwVtxIdx[2]] - pRectVtxPos[dwVtxIdx[1]]);

				return A;
			}

			// V1 + U(V2 - V1) + V(V3 - V1) 

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + dwCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pRectVtxPos[dwVtxIdx[2]],
				&pRectVtxPos[dwVtxIdx[1]],
				&pRectVtxPos[dwVtxIdx[0]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				return _vec3(pRectVtxPos[dwVtxIdx[2]].x + (pRectVtxPos[dwVtxIdx[1]].x - pRectVtxPos[dwVtxIdx[2]].x) * fU,
					0.f,
					pRectVtxPos[dwVtxIdx[2]].z + (pRectVtxPos[dwVtxIdx[0]].z - pRectVtxPos[dwVtxIdx[2]].z) * fV);
			}
		}
	}

	return _vec3{ 0,0,0 };
}
