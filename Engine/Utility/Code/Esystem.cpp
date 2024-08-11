#include "Esystem.h"

CEsystem::CEsystem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEsystem::CEsystem(const CEsystem& rhs)
	: CGameObject(rhs)
{
}

CEsystem::~CEsystem()
{
}

HRESULT CEsystem::Ready_GameObject(EFFECTTYPE pType)
{
	//컴포넌트 NULL초기화 - 그냥 바로 선언없이 멤버변수로 써도됨 
	m_pTarget = nullptr;
	m_pBufferComp = nullptr;
	m_pTextureComp = nullptr;
	m_pTransformComp = nullptr;
	m_pCalculatorComp = nullptr;
	m_pColliderComp = nullptr;
	m_pSphereComp = nullptr;

	// 타입 지정
	m_eType = pType;

	// 프레임 관련 변수 초기화 
	m_fFrame = 0.f; 
	m_fFrameEnd = 0.f; // 텍스처 받아서 벡터 사이즈로 end값 꼭 넣어주기 
	m_fFrameSpeed = 1.f; // 현재 디폴트로 1로 설정함. 
	m_fCheck = 0.f;

	// 직교투영 변수 초기화
	// 직교투영은 직접 이펙트안에서 해줘야합니다.. Component의 함수는 여기서 못써요 
	m_fX = 0.f;
	m_fY = 0.f;
	m_fSizeX = 0.f;
	m_fSizeY = 0.f;

	//matrix 초기화 
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	// 혹시 쓸일이 있을까봐 라인 ------
	m_fAge = 0.f;
	m_fLifeTime = 0.f;

	return S_OK;
}

_int CEsystem::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta); 
	
	m_fFrame += fTimeDelta * m_fFrameSpeed;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fCheck += 1;
		m_fFrame = 0.f;
	}

	return S_OK;
}

void CEsystem::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();

	// 프레임이 한번 돌았으므로 카운트 1 증가시킴 


	switch (m_eType)
	{
	case Engine::CEsystem::EFFECTTYPE::PER_SEC:
		if (m_fCurTurn >= m_fGoalTurn)
		{
			Set_Dead();
		}
		break;

	case Engine::CEsystem::EFFECTTYPE::PER_TURN:
		if (m_fAge > m_fLifeTime)
		{
			Set_Dead();
		}
		break;

	case Engine::CEsystem::EFFECTTYPE::BOTH_TYPE:
		if (m_fCurTurn >= m_fGoalTurn || m_fAge > m_fLifeTime)
		{
			Set_Dead();
		}
		break;

	}
}

void CEsystem::Free()
{
	SUPER::Free();
}
