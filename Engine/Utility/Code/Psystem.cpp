#include "Psystem.h"

CPsystem::CPsystem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CPsystem::CPsystem(const CPsystem& rhs)
	: CGameObject(rhs)
{
}

CPsystem::~CPsystem()
{
}

HRESULT CPsystem::Ready_GameObject(const _tchar* texFileName)
{
	TCHAR	szFileName[128] = L"";
	wsprintf(szFileName, texFileName);

	FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, 
		szFileName,
		(LPDIRECT3DTEXTURE9*)&m_pTexture),
		E_FAIL);

	//CVIBufferComp ���� �Ѱſ� ���� ���� ����� 
	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer(
		m_dSize * sizeof(Particle), // ������ ������ ũ��
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, // �����ϰ��� �ϴ� ���ؽ� ������ ����(0�� ��� ���� ����, D3DUSAGE_DYNAMIC)
		D3DFVF_XYZ | D3DFVF_DIFFUSE,// ���ؽ� �Ӽ� �ɼ�
		D3DPOOL_DEFAULT,// �޸� Ǯ ��� - ���� ���� (���� ������ ��� MANAGED)
		&m_pVB,// ��� �� ��ü
		0),
		E_FAIL);

	// �� ������ ���� ��ƼŬ�� �����ؾ� �ϸ� �̴� �� ���ؽ� ������ �޸𸮿� �����ؾ� ���� �ǹ�.
	// ���� ���ؽ� ���۷��� ���� �ӵ��� ����� �����Ƿ� ���� ���ؽ� ���۸� �̿�

	return S_OK;
}

_int CPsystem::Update_GameObject(const _float& fTimeDelta)
{
	for (auto& iter : m_ParticleList)
	{
		if (iter.bIsAlive)
		{
			iter.vPosition += iter.vVelocity * fTimeDelta;

			iter.fAge += fTimeDelta;

			if (iter.fAge > iter.fLifeTime)
				iter.bIsAlive = false;

			// ��� ������ ������� ���� Ȯ��
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// ��Ȱ��
				ResetParticle(&iter);
			}
		}
	}

	return S_OK;
}

void CPsystem::LateUpdate_GameObject()
{
	//RemoveDeadParticles();
}

void CPsystem::Render_GameObject()
{	// Remarks:  The render method works by filling a section of the vertex buffer with data,
	//           then we render that section.  While that section is rendering we lock a new
	//           section and begin to fill that section.  Once that sections filled we render it.
	//           This process continues until all the particles have been drawn.  The benifit
	//           of this method is that we keep the video card and the CPU busy.

	if (!m_ParticleList.empty())
	{
		//���� ���¸� ����.
		preRender();

		m_pGraphicDev->SetTexture(0, m_pTexture);
		m_pGraphicDev->SetFVF(FVF_Particle);
		m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(Particle));

		// ���ؽ� ���۸� ��� ��� ó������ �����Ѵ�.
		// m_dOffset - ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ �� ���� �ܰ���� ������(����Ʈ�� �ƴ� ��ƼŬ ����)
		// ��, �ܰ� 1�� 0���� 499���� �׸��̶�� �ܰ� 2���� �������� 500�� �ȴ�.

		if (m_dOffset >= m_dSize)
			m_dOffset = 0;

		Particle* v = 0;

		m_pVB->Lock(
			m_dOffset * sizeof(Particle),
			m_dBatchSize * sizeof(Particle),
			(void**)&v,
			m_dOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		// ��� ��ƼŬ�� �������� ������
		for (auto& iter : m_ParticleList)
		{
			if (iter.bIsAlive)
			{
				// �Ѵܰ��� ������ ��ƼŬ������ ���ؽ� ���� ���׸�Ʈ�� ����.
				v->vPosition = iter.vPosition;
				v->Color = (D3DCOLOR)iter.Color;
				v++; // next element;

				numParticlesInBatch++;  //�ܰ� ī���͸� ������Ų��.

				// ���� �ܰ谡 ��� ä���� �ִ°�?
				if (numParticlesInBatch == m_dBatchSize)
				{
					// ���ؽ� ���۷� ����� ������ �ܰ��� ��ƼŬ���� �׸���.
					m_pVB->Unlock();

					m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_dOffset, m_dBatchSize);

					// �ܰ谡 �׷����� ���� ���� �ܰ踦 ��ƼŬ�� ä���.
					// ���� �ܰ��� ó�� ���������� �̵��Ѵ�.
					m_dOffset += m_dBatchSize;

					// ���ؽ� ������ ��踦 �Ѵ¸޸𸮷� �������� �������� �ʴ´�.
					// ��踦 ���� ��� ó������ ����.
					if (m_dOffset >= m_dSize)
						m_dOffset = 0;

					m_pVB->Lock(
						m_dOffset * sizeof(Particle),
						m_dBatchSize * sizeof(Particle),
						(void**)&v,
						m_dOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // ���� �ܰ踦 ���� ����
				}
			}
		}
		m_pVB->Unlock();

		// (numParticlesInBatch == _vbBatchSize) ������ �������� ���Ͽ� ������ �ܰ谡 ���������� �ʴ� ��찡 �߻��� �� �ִ�.
		// �Ϻθ� ä���� �ܰ�� �ٷ� �̰����� ������ �ȴ�.

		if (numParticlesInBatch)
		{
			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_dOffset, numParticlesInBatch);
		}

		// next block
		m_dOffset += m_dBatchSize;

		// �������� �ʱ�ȭ 
		postRender();
	}
}


void CPsystem::AddParticle()
{
	// �ý��ۿ� ��ƼŬ�� �߰�.
	// ����Ʈ�� �߰� �ϱ����� ��ƼŬ�� �ʱ�ȭ �ϴµ� resetPaticle �̿�
	Attribute attribute;
	ResetParticle(&attribute); // �ٿ���ڽ� �ֻ������ ��ƼŬ�� �о���� 
	m_ParticleList.push_back(attribute);
}

void CPsystem::preRender()
{
	_matrix WorldMatrix;
	D3DXMatrixIdentity(&WorldMatrix);

	WorldMatrix._41 = m_vOrigin.x;
	WorldMatrix._42 = m_vOrigin.y;
	WorldMatrix._43 = m_vOrigin.z;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &WorldMatrix);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // �޸����� 
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ����Ʈ ��������Ʈ ���� ���� = �� �ϳ��� ǥ���Ǵ� ��������Ʈ, �Ϲ������� ���ڽý��ۿ� �̿� 
	// ���� ������ ��ü �ؽ�ó�� ����Ʈ ��������Ʈ�� �ؽ�ó ���ο� �̿��Ұ����� �ǹ�
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

	// ����Ʈ ũ�⸦ �� �����̽� ������ �ؼ��ϵ��� ����.
	// ����Ʈ ��������Ʈ�� ũ��� ī�޶���� �Ÿ��� ���� �����ϰ� ������.
	// ��, ī�޶�� �ָ� ������ ��ƼŬ�� ����� ��ƼŬ�� �����۰� ��Ÿ��.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	// ����Ʈ ��������Ʈ�� ũ�⸦ ����.
	// �� ���� D3DRS_POINTSCALEENABLE ���� ���� ���� �� �����̽� ���� ũ�⳪
	// ��ũ�� �����̽� ���� ũ��� �ؼ�.
	// SetRenderState ȣ���� float�� �ƴ� DWORD�� �ʿ�� �ϱ� ����.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&m_fSize));
	
	// ����Ʈ ��������Ʈ�� ������ �� �ִ� �ּ� ũ�⸦ ���� - �ʹ� �۾������ʵ��� ���� 
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, DWORD(0.0f));

	// �Ÿ��� ���� ����Ʈ ��������Ʈ�� ũ�Ⱑ ���ϴ� ����� ����
	// ���⿡�� ���ϴ� �Ÿ��� ī�޶�� ����Ʈ ��������Ʈ ���� �Ÿ�.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, DWORD(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, DWORD(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&m_fSize));

	//// �ؽ�ó�� ���ĸ� �̿�
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //��
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// ���� ������ Ȱ��ȭ�Ͽ� �ؽ�ó�� ���� ä���� �ؽ�ó�� �ȼ� ������ �����ϵ��� ����.
	// �̸� ���� �پ��� ȿ���� ���� �� ������,
	// ���� ��ǥ���� ���� �簢���� �ƴ� �ٸ� ��ƼŬ ���¸� ���� �ϴ� ��.
	// ��, "�����̿� �����" �ձ� ��ƼŬ�� ��� ���ؼ���
	// ����� ������ �������� ���� ä���� ���� ��� �ؽ�ó�� �̿��ϸ� ��.
	// �̷��� �ϸ� �簢���� ��� �ؽ�ó ��ü���ƴ� ��� �� ����� ��ƼŬ�� ���� �� �ִ�.
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

// Ư�� ��ƼŬ �ý����� �������� �� �ִ� ���� ���¸� �����ϴ� �� �̿�.
// �� �޼���� �ý��ۿ� ���� �޶��� �� �����Ƿ� ���� �޼���� ����.
void CPsystem::postRender()
{
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CPsystem::ResetList()
{
	for (auto& iter : m_ParticleList)
	{
		ResetParticle(&iter);
	}
}

bool CPsystem::isDead()
{
	for (const auto& attribute : m_ParticleList)
	{
		if (attribute.bIsAlive)
			return false;
	}
	return true;
}

void CPsystem::RemoveDeadParticles()
{
	for (auto iter = m_ParticleList.begin(); iter != m_ParticleList.end();)
	{
		if ((iter)->bIsAlive == false)
		{
			// erase�� ���� �ݺ��ڸ� �����ϹǷ�
			// �츮�� �ݺ��ڸ� ������ų �ʿ䰡 ����.
			iter = m_ParticleList.erase(iter);
		}
		else
			++iter;
	}
}

void CPsystem::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pTexture);
	RemoveDeadParticles(); // ����Ʈ erase
	SUPER::Free(); 
	//for_each(m_ParticleList.begin(), m_ParticleList.end(), CDeleteObj());
	//m_ParticleList.clear();
}
