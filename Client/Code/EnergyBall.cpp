#include "stdafx.h"
#include "EnergyBall.h"

#include "Player.h"

CEnergyBall::CEnergyBall(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CEnergyBall::CEnergyBall(const CEnergyBall& rhs)
	: Base(rhs)
{
}

CEnergyBall::~CEnergyBall()
{
}

CEnergyBall* CEnergyBall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _CurrPhase, CGameObject* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Value_Setting(_x, _y, _z, _CurrPhase);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CEnergyBall::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//기본셋팅 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"EnergyBall");
	m_pTransformComp->Set_Scale({ 0.7f, 0.7f, 0.7f });
	m_fAttack = 4.f;

	// 프레임 및 사망시간 조정
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fRepeat = 0.f;
	m_tFrame.fAge = 0.f;
	// LifeTime은 패턴강화에서 설정 

	// 충돌용
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());

	Engine::Play_Sound(L"Enemy", L"_EnergyBallA.wav", SOUND_ENEMY_EFFECT, 0.4f);

	return S_OK;
}

_int CEnergyBall::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	Billboard();

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;
	m_tFrame.fAge += fTimeDelta * 1.f;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 4;
	}

	if (m_tFrame.fAge < m_fGuideTime)
		Follow_Player(fTimeDelta);
	else
		m_pTransformComp->Move_Pos(&Dir, fTimeDelta, m_fMovingSpeed);

	if (m_tFrame.fAge > m_tFrame.fLifeTime)
		Set_Dead();
		

	//물리 업데이트 코드
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 콜라이더 위치 업데이트 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

#pragma region 기본셋팅 

void CEnergyBall::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CEnergyBall::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEnergyBall::Add_Component()
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
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // 얘랑 충돌해야하는 레이어들 

	return S_OK;
}

void CEnergyBall::Free()
{
	SUPER::Free();
}

#pragma endregion

#pragma region 충돌 

void CEnergyBall::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CEnergyBall::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"▶EnergyBall 충돌 \n");

	if (Attack_Occurrence(pDst, m_fAttack))
		Set_Dead();

}

void CEnergyBall::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion
HRESULT CEnergyBall::Follow_Player(const _float fTimeDelta)
{

	// 이펙트가 플레이어를 보는 벡터 
	Dir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&Dir, &Dir);

	m_pTransformComp->Move_Pos(&Dir, fTimeDelta, m_fMovingSpeed);

	return S_OK;

}

void CEnergyBall::Value_Setting(_float _x, _float _y, _float _z, MonsterPhase _phase)
{
	switch (_phase)
	{
	case Engine::Phase1:
		m_tFrame.fFrameSpeed = 6.f;
		m_fMovingSpeed = 3.f;
		m_tFrame.fLifeTime = 4.f;
		m_fGuideTime = 2.f;
		
		break;

	case Engine::Phase2:
		m_tFrame.fFrameSpeed = 8.f;
		m_fMovingSpeed = 5.f;
		m_fGuideTime = 3.f;
		m_tFrame.fLifeTime = 6.f;
		break;

	}
}
