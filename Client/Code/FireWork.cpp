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

	m_vOrigin = vOriginPos;			// �ý��� ������ ��ƼŬ�� ���۵Ǵ� ��.
	m_fSize = 1.f;					// �ý��� �� ��� ��ƼŬ�� ũ��
	m_dSize = 4096;					// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��- ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
	m_dOffset = 0;					// ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ �� ���� �ܰ���� ������(����Ʈ�� �ƴ� ��ƼŬ ����)
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

			// �ٿ�� �ڽ� ���� 
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// ��Ȱ��
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

	//// ���̴� ========================================
	//gpLightingShader->SetMatrix("gWorldMatrix", &matWorld);
	//gpLightingShader->SetMatrix("gViewMatrix", &matView);
	//gpLightingShader->SetMatrix("gProjectionMatrix", &matProjection);

	//gpLightingShader->SetVector("gWorldLightPosition", &gWorldLightPosition);
	//gpLightingShader->SetVector("gWorldCameraPosition", &gWorldCameraPosition);
	
	//// ���̴��� �����Ѵ�.
	//_uint numPasses = 0;
	//gpLightingShader->Begin(&numPasses, NULL);
	//{
	//	for (_uint i = 0; i < numPasses; ++i)
	//	{
	//		gpLightingShader->BeginPass(i);
	//		{
	//			// ��ü�� �׸���.
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
	//			// ��ü�� �׸���.
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
	// �ý��� ��õ(�Ҳ��� ������ ���) ������ ������, �ý����� ���� ��ƼŬ�� ��.
	// �ý��� ��õ�� ��ƼŬ�� �ʱ�ȭ�ϰ� ��ü ������ ������ �ӵ��� �����,
	// �ý��� ���� ��ƼŬ���� ������ �÷��� �ο�.
	// �� ��ƼŬ�� ������ ����.
	_attribute->bIsAlive = true;

	_attribute->vPosition = m_vOrigin;
	_vec3 vMin = _vec3(-1.0f, -1.0f, -1.f);
	_vec3 vMax = _vec3(1.0f, 1.0f, 1.f);

	Get_RandomVector(&_attribute->vVelocity, &vMin, &vMax);

	// ��ü�� ����� ���� ����ȭ
	D3DXVec3Normalize( &_attribute->vVelocity, &_attribute->vVelocity);

	_attribute->vVelocity *= 10.0f;

	_attribute->Color = D3DXCOLOR(	1.0f, // R
									Get_RandomFloat(0.0f, 1.0f), // G
									Get_RandomFloat(0.0f, 1.0f), // B
									1.0f); // A
	//Get_RandomFloat(0.0f, 1.0f) rgb
	_attribute->fAge = 0.0f;
	_attribute->fLifeTime = 2.0f; //2���� ������ ������. 

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

	// ���̴� �ε��� ������ ��� outputâ�� ���̴�
	// ������ ������ ����Ѵ�.

	if (FAILED(result) && pError)
	{
		_int size = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			// WCHAR ������ ���ڿ��� ����Ϸ��� wprintf�� ����ؾ� �մϴ�.
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
	//// ���̴� �ε� - L4
	//gpLightingShader = LoadShader(L"./Resource/Texture/Particle/Lighting.fx");
	//if (!gpLightingShader)
	//{
	//	return false;
	//}
	
	// ���̴� �ε� - L3
	gpTextureMappingShader = LoadShader(L"./Resource/Texture/Particle/TextureMapping.fx");
	if (!gpTextureMappingShader)
	{
		return false;
	}

	// �� �ε�
	gpSphere = LoadModel(L"./Resource/Texture/Particle/Sphere.x");
	if (!gpSphere)
	{
		return false;
	}

	// �ؽ�ó �ε�
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

	// ���� release �Ѵ�.
	if (gpSphere)
	{
		gpSphere->Release();
		gpSphere = NULL;
	}

	// ���̴��� release �Ѵ�.
	if (gpLightingShader)
	{
		gpLightingShader->Release();
		gpLightingShader = NULL;
	}
}