#include "stdafx.h"
#include "MonsterPunch.h"
#include "AceBoss.h"
#include "Gray.h"

CMonsterPunch::CMonsterPunch(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CMonsterPunch::CMonsterPunch(const CMonsterPunch& rhs)
	: Base(rhs)
{
}

CMonsterPunch::~CMonsterPunch()
{
}

CMonsterPunch* CMonsterPunch::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, TYPE _option, CAceUnit* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("MonsterPunch Create Failed");
		return nullptr;
	}

	// 생성할때 몬스터 위치 로 생성하기 위해 Create에서 초기위치를 잡아줌 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->m_eAttackType = _option;
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CMonsterPunch::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 기본셋팅
	m_fAttack = 3.f;
	m_fHeavyAttack = 5.f;

	// 충돌 - 구형 
	m_pTransformComp->Readjust_Transform();
	pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pSphereShape->fRadius = 1.f;

	//디버그용 텍스쳐
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Debug", L"Sphere");

	return S_OK;
}

_int CMonsterPunch::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	//충돌하지 않았을경우 없앤다. 
	m_tFrame.fAge += fTimeDelta * 1.f;

	if (m_tFrame.fAge > m_tFrame.fLifeTime)
	{
		Set_Dead();
	}

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CMonsterPunch::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CMonsterPunch::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

#pragma region 충돌 메쉬 콜라이더
	//MeshSphereColider(_float(pSphereShape->fRadius), 8, 16);
#pragma endregion

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonsterPunch::Add_Component()
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
	m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER_ATTACK); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER); // 얘랑 충돌해야하는 레이어들 

	return S_OK;
}

void CMonsterPunch::Free()
{
	SUPER::Free();
}

void CMonsterPunch::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CMonsterPunch::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(m_pOwner);

	CGray* pGray = dynamic_cast<CGray*>(pAceObj);
	
	if (nullptr == pGray)
	{
		Engine::Play_Sound(L"Enemy", L"_HitPunch.mp3", SOUND_ENEMY_NORMAL_EFFECT, 0.6f);
	}
	else
	{
		// Gray가 만든것 
		Engine::Play_Sound(L"Enemy", L"_PipeHit.wav", SOUND_ENEMY_NORMAL_EFFECT, 0.6f);
	}
	

	switch (m_eAttackType)
	{
	case CMonsterPunch::TYPE::NORMAL:
		Attack_Occurrence(pDst, m_fAttack);
		break;

	case CMonsterPunch::TYPE::HEAVY:
		Attack_Occurrence(pDst, m_fHeavyAttack);
		break;
	}

	Set_Dead();
}

void CMonsterPunch::OnCollisionExited(CGameObject* pDst)
{
}