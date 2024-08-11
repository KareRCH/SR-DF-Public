#include "stdafx.h"
#include "FireWork.h"

CFireWork::CFireWork(LPDIRECT3DDEVICE9 pGraphicDev)
	:CPsystem(pGraphicDev)
{
}

CFireWork::~CFireWork()
{
}

HRESULT CFireWork::Ready_GameObject(_vec3 vOriginPos, _int numParticles)
{
	//FAILED_CHECK_RETURN(LoadAssets(), E_FAIL);

	m_vOrigin = vOriginPos;			// 시스템 내에서 파티클이 시작되는 곳.
	m_fSize = 1.f;					// 시스템 내 모든 파티클의 크기
	m_dSize = 4096;					// 버텍스 버퍼가 보관할 수 있는 파티클의 수- 실제 파티클 시스템 내의 파티클 수와는 독립적.
	m_dOffset = 0;					// 버텍스 버퍼에서 복사를 시작할 파티클 내 다음 단계로의 오프셋(바이트가 아닌 파티클 단위)
	m_dBatchSize = 512;
	m_BoundingBox.vMin = _vec3(-50.0f, -50.0f, -50.0f);
	m_BoundingBox.vMax = _vec3(150.0f, 50.0f, 150.0f);

	for (int i = 0; i < numParticles; i++)
		AddParticle();

	//_tchar* pPath = L"./Resource/Texture/Particle/snowball.bmp";
	//_tchar* pPath = L"./Resource/Texture/Particle/crystal.bmp";
	//_tchar* pPath = L"./Resource/Texture/Particle/heart.bmp";
	_tchar* pPath = L"./Resource/Texture/Particle/flower.png";
	//_tchar* pPath = L"./Resource/Texture/Particle/DefaultParticle.png";

	CPsystem::Ready_GameObject(pPath);
	return S_OK;
}

CFireWork* CFireWork::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, _int numParticles)
{
	CFireWork* pInstance = new CFireWork(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vOriginPos, numParticles)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Particle FireWork Create Failed");
		return nullptr;
	}

	return pInstance;
}

_int CFireWork::Update_GameObject(const _float& fTimeDelta)
{
	m_fTime += 1.f * fTimeDelta;

	for (auto& iter : m_ParticleList)
	{
		if (iter.bIsAlive)
		{
			iter.vPosition += iter.vVelocity * fTimeDelta;

			iter.fAge += fTimeDelta;

			if (iter.fAge > iter.fLifeTime)
				iter.bIsAlive = false;

			// 바운딩 박스 범위 
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// 재활용
				ResetParticle(&iter);
			}

			if (Engine::IsKey_Pressed(DIK_X))
			{
				m_fDieTime = 100.f;
				ResetParticle(&iter);
			}
		}
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CFireWork::LateUpdate_GameObject(void)
{
	SUPER::LateUpdate_GameObject();
}

void CFireWork::Render_GameObject()
{
	CPsystem::Render_GameObject();
	
	/*_matrix matWorld, matView, matProjection;
	m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProjection);*/

	//// 셰이더 ========================================
	//gpLightingShader->SetMatrix("gWorldMatrix", &matWorld);
	//gpLightingShader->SetMatrix("gViewMatrix", &matView);
	//gpLightingShader->SetMatrix("gProjectionMatrix", &matProjection);

	//gpLightingShader->SetVector("gWorldLightPosition", &gWorldLightPosition);
	//gpLightingShader->SetVector("gWorldCameraPosition", &gWorldCameraPosition);
	
	//// 쉐이더를 시작한다.
	//_uint numPasses = 0;
	//gpLightingShader->Begin(&numPasses, NULL);
	//{
	//	for (_uint i = 0; i < numPasses; ++i)
	//	{
	//		gpLightingShader->BeginPass(i);
	//		{
	//			// 구체를 그린다.
	//			gpSphere->DrawSubset(0);
	//		}
	//		gpLightingShader->EndPass();
	//	}
	//}
	//gpLightingShader->End();

	//UINT numPasses = 0;
	//gpTextureMappingShader->Begin(&numPasses, NULL);
	//{
	//	for (UINT i = 0; i < numPasses; ++i)
	//	{
	//		gpTextureMappingShader->BeginPass(i);
	//		{
	//			// 구체를 그린다.
	//			gpSphere->DrawSubset(0);
	//		}
	//		gpTextureMappingShader->EndPass();
	//	}
	//}
	//gpTextureMappingShader->End();

	//m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CFireWork::ResetParticle(Attribute* _attribute)
{
	// 시스템 원천(불꽃이 폭발할 장소) 으로의 포인터, 시스템이 가진 파티클의 수.
	// 시스템 원천의 파티클을 초기화하고 구체 내에서 임의의 속도를 만들며,
	// 시스템 내의 파티클들은 임의의 컬러를 부여.
	// 각 파티클들 수명을 지정.
	_attribute->bIsAlive = true;

	_attribute->vPosition = m_vOrigin;
	_vec3 vMin = _vec3(-1.0f, -1.0f, -1.f);
	_vec3 vMax = _vec3(1.0f, 1.0f, 1.f);

	Get_RandomVector(&_attribute->vVelocity, &vMin, &vMax);

	// 구체를 만들기 위한 정규화
	D3DXVec3Normalize( &_attribute->vVelocity, &_attribute->vVelocity);

	_attribute->vVelocity *= 10.0f;

	_attribute->Color = D3DXCOLOR(	1.0f, // R
									Get_RandomFloat(0.0f, 1.0f), // G
									Get_RandomFloat(0.0f, 1.0f), // B
									1.0f); // A
	//Get_RandomFloat(0.0f, 1.0f) rgb
	_attribute->fAge = 0.0f;
	_attribute->fLifeTime = 2.0f; //2초의 수명을 가진다. 

	//m_fDieTime = Get_RandomFloat(3.f, 10.f);
	//m_fMoveTime = Get_RandomFloat(.1f, .5f);
}

LPD3DXEFFECT CFireWork::LoadShader(const _tchar* filename)
{
	LPD3DXEFFECT ret = NULL;

	LPD3DXBUFFER pError = NULL;
	DWORD dwShaderFlags = 0;

#if _DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	HRESULT result = D3DXCreateEffectFromFile(m_pGraphicDev, filename,
		NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

	// 쉐이더 로딩에 실패한 경우 output창에 쉐이더
	// 컴파일 에러를 출력한다.

	if (FAILED(result) && pError)
	{
		_int size = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			// WCHAR 형식의 문자열을 출력하려면 wprintf를 사용해야 합니다.
			wprintf(L"%.*s\n", size, static_cast<WCHAR*>(ack));
		}
	}

	return ret;
}

LPD3DXMESH CFireWork::LoadModel(const _tchar* filename)
{
	LPD3DXMESH _3DMesh = NULL;

	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, m_pGraphicDev, NULL, NULL, NULL, NULL, &_3DMesh)))
	{
		MSG_BOX("Model Loading Failed");
	};

	return _3DMesh;
}

LPDIRECT3DTEXTURE9 CFireWork::LoadTexture(const _tchar* filename)
{
	LPDIRECT3DTEXTURE9 _3DTexture = NULL;

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, filename, &_3DTexture)))
	{
		MSG_BOX("Texture Loading Failed");
	}

	return _3DTexture;
}

_bool CFireWork::LoadAssets()
{
	//// 쉐이더 로딩 - L4
	//gpLightingShader = LoadShader(L"./Resource/Texture/Particle/Lighting.fx");
	//if (!gpLightingShader)
	//{
	//	return false;
	//}
	
	// 쉐이더 로딩 - L3
	gpTextureMappingShader = LoadShader(L"./Resource/Texture/Particle/TextureMapping.fx");
	if (!gpTextureMappingShader)
	{
		return false;
	}

	// 모델 로딩
	gpSphere = LoadModel(L"./Resource/Texture/Particle/Sphere.x");
	if (!gpSphere)
	{
		return false;
	}

	// 텍스처 로딩
	gpEarthDM = LoadTexture(L"./Resource/Texture/Particle/Earth.jpg");
	if (!gpEarthDM)
	{
		return false;
	}

	return true;
}

void CFireWork::Free()
{
	SUPER::Free();

	// 모델을 release 한다.
	if (gpSphere)
	{
		gpSphere->Release();
		gpSphere = NULL;
	}

	// 쉐이더를 release 한다.
	if (gpLightingShader)
	{
		gpLightingShader->Release();
		gpLightingShader = NULL;
	}
}