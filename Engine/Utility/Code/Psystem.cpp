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

	//CVIBufferComp 에서 한거와 같은 버퍼 만들기 
	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer(
		m_dSize * sizeof(Particle), // 생성할 버퍼의 크기
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, // 생성하고자 하는 버텍스 버퍼의 종류(0인 경우 정적 버퍼, D3DUSAGE_DYNAMIC)
		D3DFVF_XYZ | D3DFVF_DIFFUSE,// 버텍스 속성 옵션
		D3DPOOL_DEFAULT,// 메모리 풀 방식 - 동적 생성 (정적 버퍼인 경우 MANAGED)
		&m_pVB,// 결과 컴 객체
		0),
		E_FAIL);

	// 매 프레임 마다 파티클을 갱신해야 하며 이는 곧 버텍스 버퍼의 메모리에 접근해야 함을 의미.
	// 정적 버텍스 버퍼로의 접근 속도는 상당히 느리므로 동적 버텍스 버퍼를 이용

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

			// 경계 범위를 벗어났는지 여부 확인
			if (m_BoundingBox.vMax.x < iter.vPosition.x || m_BoundingBox.vMin.x > iter.vPosition.x ||
				m_BoundingBox.vMax.y < iter.vPosition.y || m_BoundingBox.vMin.y > iter.vPosition.y ||
				m_BoundingBox.vMax.z < iter.vPosition.z || m_BoundingBox.vMin.z > iter.vPosition.z)
			{
				// 재활용
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
		//렌더 상태를 지정.
		preRender();

		m_pGraphicDev->SetTexture(0, m_pTexture);
		m_pGraphicDev->SetFVF(FVF_Particle);
		m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(Particle));

		// 버텍스 버퍼를 벗어날 경우 처음부터 시작한다.
		// m_dOffset - 버텍스 버퍼에서 복사를 시작할 파티클 내 다음 단계로의 오프셋(바이트가 아닌 파티클 단위)
		// 예, 단계 1이 0부터 499까지 항목이라면 단계 2로의 오프셋은 500이 된다.

		if (m_dOffset >= m_dSize)
			m_dOffset = 0;

		Particle* v = 0;

		m_pVB->Lock(
			m_dOffset * sizeof(Particle),
			m_dBatchSize * sizeof(Particle),
			(void**)&v,
			m_dOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		// 모든 파티클이 렌더링될 때까지
		for (auto& iter : m_ParticleList)
		{
			if (iter.bIsAlive)
			{
				// 한단계의 생존한 파티클을다음 버텍스 버퍼 세그먼트로 복사.
				v->vPosition = iter.vPosition;
				v->Color = (D3DCOLOR)iter.Color;
				v++; // next element;

				numParticlesInBatch++;  //단계 카운터를 증가시킨다.

				// 현재 단계가 모두 채워져 있는가?
				if (numParticlesInBatch == m_dBatchSize)
				{
					// 버텍스 버퍼로 복사된 마지막 단계의 파티클들을 그린다.
					m_pVB->Unlock();

					m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_dOffset, m_dBatchSize);

					// 단계가 그려지는 동안 다음 단계를 파티클로 채운다.
					// 다음 단계의 처음 오프셋으로 이동한다.
					m_dOffset += m_dBatchSize;

					// 버텍스 버퍼의 경계를 넘는메모리로 오프셋을 설정하지 않는다.
					// 경계를 넘을 경우 처음부터 시작.
					if (m_dOffset >= m_dSize)
						m_dOffset = 0;

					m_pVB->Lock(
						m_dOffset * sizeof(Particle),
						m_dBatchSize * sizeof(Particle),
						(void**)&v,
						m_dOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // 다음 단계를 위한 리셋
				}
			}
		}
		m_pVB->Unlock();

		// (numParticlesInBatch == _vbBatchSize) 조건이 만족되지 못하여 마지막 단계가 렌더링되지 않는 경우가 발생할 수 있다.
		// 일부만 채워진 단계는 바로 이곳에서 렌더링 된다.

		if (numParticlesInBatch)
		{
			m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_dOffset, numParticlesInBatch);
		}

		// next block
		m_dOffset += m_dBatchSize;

		// 렌더상태 초기화 
		postRender();
	}
}


void CPsystem::AddParticle()
{
	// 시스템에 파티클을 추가.
	// 리스트에 추가 하기전에 파티클을 초기화 하는데 resetPaticle 이용
	Attribute attribute;
	ResetParticle(&attribute); // 바운딩박스 최상단으로 파티클을 밀어넣음 
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
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // 뒷면제거 
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// 포인트 스프라이트 렌더 상태 = 점 하나로 표현되는 스프라이트, 일반적으로 입자시스템에 이용 
	// 현재 지정된 전체 텍스처를 포인트 스프라이트의 텍스처 매핑에 이용할것임을 의미
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

	// 포인트 크기를 뷰 스페이스 단위로 해석하도록 지정.
	// 포인트 스프라이트의 크기는 카메라와의 거리에 따라 적절하게 조정됨.
	// 즉, 카메라와 멀리 떨어진 파티클은 가까운 파티클에 비해작게 나타남.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	// 포인트 스프라이트의 크기를 지정.
	// 이 값은 D3DRS_POINTSCALEENABLE 상태 값에 따라서 뷰 스페이스 내의 크기나
	// 스크린 스페이스 내의 크기로 해석.
	// SetRenderState 호출이 float이 아닌 DWORD를 필요로 하기 때문.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&m_fSize));
	
	// 포인트 스프라이트의 지정할 수 있는 최소 크기를 지정 - 너무 작아지지않도록 제한 
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, DWORD(0.0f));

	// 거리에 따라 포인트 스프라이트의 크기가 변하는 방법을 제어
	// 여기에서 말하는 거리란 카메라와 포인트 스프라이트 간의 거리.
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, DWORD(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, DWORD(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&m_fSize));

	//// 텍스처의 알파를 이용
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //◀
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 알파 블렌딩을 활성화하여 텍스처의 알파 채널이 텍스처의 픽셀 투명도를 결정하도록 했음.
	// 이를 통해 다양한 효과를 얻을 수 있으며,
	// 가장 대표적인 것이 사각형이 아닌 다른 파티클 형태를 구현 하는 것.
	// 예, "눈덩이와 비슷한" 둥근 파티클을 얻기 위해서는
	// 흰색의 원형과 검은색의 알파 채널을 갖는 흰색 텍스처를 이용하면 됨.
	// 이렇게 하면 사각형의 흰색 텍스처 전체가아닌 흰색 원 모양의 파티클을 만들 수 있다.
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

// 특정 파티클 시스템이 지정했을 수 있는 렌더 상태를 복구하는 데 이용.
// 이 메서드는 시스템에 따라 달라질 수 있으므로 가상 메서드로 선언.
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
			// erase는 다음 반복자를 리턴하므로
			// 우리가 반복자를 증가시킬 필요가 없다.
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
	RemoveDeadParticles(); // 리스트 erase
	SUPER::Free(); 
	//for_each(m_ParticleList.begin(), m_ParticleList.end(), CDeleteObj());
	//m_ParticleList.clear();
}
