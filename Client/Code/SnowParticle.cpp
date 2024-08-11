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

	m_vOrigin = vOriginPos;			// �ý��� ������ ��ƼŬ�� ���۵Ǵ� ��.
	m_fSize = 0.15f;					// �ý��� �� ��� ��ƼŬ�� ũ��
	m_dSize = 4096;					// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��- ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
	m_dOffset = 0;					// ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ �� ���� �ܰ���� ������(����Ʈ�� �ƴ� ��ƼŬ ����)
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

			// ��� ������ ����°�?
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// ��Ȱ��
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

	// GetRandomVector - �ּ��� min�� �ִ��� max�� ���ǵ� ���� ���� ���� ���͸� ���.
	// �������� ��ġ ������ ���� ������ x, z ��ǥ�� ��´�.
	Get_RandomVector(&_attribute->vPosition, &m_BoundingBox.vMin, &m_BoundingBox.vMax);

	// ���� (y��ǥ) �� �׻� ��� ������ �ֻ��.
	_attribute->vPosition.y = m_BoundingBox.vMax.y;

	// GetRandowFloat - [ lowBound, highBound ] ������ �ִ� ������ float�� ����.
	 // �����̴� �Ʒ������� �������� �ణ ������ ���Ѵ�.
	_attribute->vVelocity.x = Get_RandomFloat(0.0f, 1.0f) * -4.0f;
	_attribute->vVelocity.y = Get_RandomFloat(0.0f, 1.0f) * -12.0f;
	_attribute->vVelocity.z = 0.0f;

	_float a = Get_RandomFloat(0.9f, 1.0f);
	// ����� ������	
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
