#include "stdafx.h"
#include "SnowParticle.h"

CSnowParticle::CSnowParticle(LPDIRECT3DDEVICE9 pGraphicDev)
	:CPsystem(pGraphicDev)
{
}

CSnowParticle::~CSnowParticle()
{
}

HRESULT CSnowParticle::Ready_GameObject(_vec3 vOriginPos, _int numParticles)
{
	srand(_ulong(time(NULL)));
	BoundingBox boundingBox;
	boundingBox.vMin = _vec3(-50.0f, -10.0f, -50.0f);
	boundingBox.vMax = _vec3(150.0f, 50.0f, 150.0f);

	m_vOrigin = vOriginPos;			// 시스템 내에서 파티클이 시작되는 곳.
	m_fSize = 0.15f;					// 시스템 내 모든 파티클의 크기
	m_dSize = 4096;					// 버텍스 버퍼가 보관할 수 있는 파티클의 수- 실제 파티클 시스템 내의 파티클 수와는 독립적.
	m_dOffset = 0;					// 버텍스 버퍼에서 복사를 시작할 파티클 내 다음 단계로의 오프셋(바이트가 아닌 파티클 단위)
	m_dBatchSize = 512;
	m_BoundingBox = boundingBox;  

	for (int i = 0; i < numParticles; i++)
		AddParticle();

	//_tchar* pPath = L"./Resource/Texture/Particle/snowball.bmp";
	_tchar* pPath = L"./Resource/Texture/Particle/CrystalSnow2.bmp";

	CPsystem::Ready_GameObject(pPath);

	return S_OK;
}

_int CSnowParticle::Update_GameObject(const _float& fTimeDelta)
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

			// 경계 범위를 벗어났는가?
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// 재활용
				ResetParticle(&iter);
			}
		}
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return 0;
}

void CSnowParticle::LateUpdate_GameObject(void)
{
	SUPER::LateUpdate_GameObject();
}

void CSnowParticle::Render_GameObject()
{
	CPsystem::Render_GameObject();
}

void CSnowParticle::ResetParticle(Attribute* _attribute)
{
	_attribute->bIsAlive = true;

	// GetRandomVector - 최소점 min과 최대점 max로 정의된 상자 내의 임의 벡터를 출력.
	// 눈송이의 위치 지정을 위해 임의의 x, z 좌표를 얻는다.
	Get_RandomVector(&_attribute->vPosition, &m_BoundingBox.vMin, &m_BoundingBox.vMax);

	// 높이 (y좌표) 는 항상 경계 상자의 최상단.
	_attribute->vPosition.y = m_BoundingBox.vMax.y;

	// GetRandowFloat - [ lowBound, highBound ] 범위에 있는 임의의 float을 리턴.
	 // 눈송이는 아래쪽으로 떨어지며 약간 왼쪽을 향한다.
	_attribute->vVelocity.x = Get_RandomFloat(0.0f, 1.0f) * -4.0f;
	_attribute->vVelocity.y = Get_RandomFloat(0.0f, 1.0f) * -12.0f;
	_attribute->vVelocity.z = 0.0f;

	_float a = Get_RandomFloat(0.9f, 1.0f);
	// 흰색의 눈송이	
	_attribute->Color = D3DXCOLOR(a, // R
								  a, // G
								  a, // B
								  1.0f); // A
	_attribute->fLifeTime = 60.f;
	_attribute->fAge = 0.f;
}

CSnowParticle* CSnowParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, _int numParticles)
{
	CSnowParticle* pInstance = new CSnowParticle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(vOriginPos, numParticles)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Particle Snow Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSnowParticle::Free()
{
	SUPER::Free();
}
