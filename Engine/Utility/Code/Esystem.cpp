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
	//������Ʈ NULL�ʱ�ȭ - �׳� �ٷ� ������� ��������� �ᵵ�� 
	m_pTarget = nullptr;
	m_pBufferComp = nullptr;
	m_pTextureComp = nullptr;
	m_pTransformComp = nullptr;
	m_pCalculatorComp = nullptr;
	m_pColliderComp = nullptr;
	m_pSphereComp = nullptr;

	// Ÿ�� ����
	m_eType = pType;

	// ������ ���� ���� �ʱ�ȭ 
	m_fFrame = 0.f; 
	m_fFrameEnd = 0.f; // �ؽ�ó �޾Ƽ� ���� ������� end�� �� �־��ֱ� 
	m_fFrameSpeed = 1.f; // ���� ����Ʈ�� 1�� ������. 
	m_fCheck = 0.f;

	// �������� ���� �ʱ�ȭ
	// ���������� ���� ����Ʈ�ȿ��� ������մϴ�.. Component�� �Լ��� ���⼭ ����� 
	m_fX = 0.f;
	m_fY = 0.f;
	m_fSizeX = 0.f;
	m_fSizeY = 0.f;

	//matrix �ʱ�ȭ 
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	// Ȥ�� ������ ������� ���� ------
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

	// �������� �ѹ� �������Ƿ� ī��Ʈ 1 ������Ŵ 


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
