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

	// �����Ҷ� ���� ��ġ �� �����ϱ� ���� Create���� �ʱ���ġ�� ����� 
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

	// �⺻����
	m_fAttack = 3.f;
	m_fHeavyAttack = 5.f;

	// �浹 - ���� 
	m_pTransformComp->Readjust_Transform();
	pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pSphereShape->fRadius = 1.f;

	//����׿� �ؽ���
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Debug", L"Sphere");

	return S_OK;
}

_int CMonsterPunch::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	//�浹���� �ʾ������ ���ش�. 
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

#pragma region �浹 �޽� �ݶ��̴�
	//MeshSphereColider(_float(pSphereShape->fRadius), 8, 16);
#pragma endregion

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonsterPunch::Add_Component()
{
	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER_ATTACK); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER); // ��� �浹�ؾ��ϴ� ���̾�� 

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
		// Gray�� ����� 
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