#include "stdafx.h"
#include "SlowThunder.h"

#include "Player.h"

CSlowThunder::CSlowThunder(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CSlowThunder::CSlowThunder(const CSlowThunder& rhs)
	: Base(rhs)
{
}

CSlowThunder::~CSlowThunder()
{
}

CSlowThunder* CSlowThunder::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAceUnit* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CSlowThunder::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 기본셋팅
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"SlowThunder");
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });
	m_fAttack = 7.f;

	// 프레임 및 사망시간 조정
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 8.f;

	// 크기조정
	// 충돌용
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.5f;

	return S_OK;
}

_int CSlowThunder::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	Billboard();

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fRepeat += 1;
		m_tFrame.fFrame = 0.f;

		if (3 == m_tFrame.fRepeat)
		{
			m_tFrame.fFrame = m_tFrame.fFrameEnd - 1.f;
			Set_Dead();
		}
	}
	

	//물리 업데이트 코드
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 콜라이더 위치 업데이트 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CSlowThunder::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CSlowThunder::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

#pragma region 기본셋팅 

HRESULT CSlowThunder::Add_Component()
{
	// 콜라이더 컴포넌트
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	// 물리 세계 등록
	m_pColliderComp->EnterToPhysics(0);

	// 충돌 함수 연결
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// 충돌 레이어, 마스크 설정
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSS_SKILL); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER ); // 얘랑 충돌해야하는 레이어들 

	return S_OK;
}

void CSlowThunder::Free()
{
	SUPER::Free();
}

#pragma endregion

void CSlowThunder::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
	//m_pTransformComp
}

void CSlowThunder::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"▶SlowThunder 충돌 \n");

	if (Attack_Occurrence(pDst, m_fAttack))
		Set_Dead();
}

void CSlowThunder::OnCollisionExited(CGameObject* pDst)
{
}
