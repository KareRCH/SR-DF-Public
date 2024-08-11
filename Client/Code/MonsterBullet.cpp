#include "stdafx.h"
#include "MonsterBullet.h"

CMonsterBullet::CMonsterBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CMonsterBullet::CMonsterBullet(const CMonsterBullet& rhs)
	: Base(rhs)
{
}

CMonsterBullet::~CMonsterBullet()
{
}

CMonsterBullet* CMonsterBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, 
										TYPE _option, CAceUnit* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("MonsterBullet Create Failed");
		return nullptr;
	}

	// �����Ҷ� ���� ��ġ �� �����ϱ� ���� Create���� �ʱ���ġ�� ����� 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->m_eAttackType = _option;
	pInstance->Basic_Setting(_option);
	pInstance->Set_TeamID(_eTeamid);
	pInstance->Set_Owner(pOwner);

	return pInstance;
}

HRESULT CMonsterBullet::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �浹
	m_pTransformComp->Readjust_Transform();
	pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pSphereShape->fRadius = 0.3f;

	//����׿� �ؽ���
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"Ammo");
	m_pTransformComp->Set_Scale({ 0.3f, 0.3f, 0.3f });
	return S_OK;
}

_int CMonsterBullet::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	m_pTransformComp->Move_Pos(&vDir, fTimeDelta, m_fMovingSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CMonsterBullet::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CMonsterBullet::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));
	m_pTextureComp->Render_Texture();
	m_pBufferComp->Render_Buffer();

#pragma region �浹 �޽� �ݶ��̴�
	//MeshSphereColider(_float(pSphereShape->fRadius), 32, 16);
#pragma endregion

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CMonsterBullet::Add_Component()
{
	CTransformComponent* m_pBossTransformComp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Boss", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pBossTransformComp, -1);

	vDir = m_pBossTransformComp->Get_Look();
	D3DXVec3Normalize(&vDir, &vDir);

	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_PROJECTILE); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CMonsterBullet::Basic_Setting(TYPE _option)
{
	if (TYPE::NORMAL == _option)
	{
		m_fAttack = 4.f;
		m_fMovingSpeed = 17.f;
	}
	else if (TYPE::HEAVY == _option)
	{
		m_fAttack = 4.f;
		m_fMovingSpeed = 23.f;
	}
}

void CMonsterBullet::Free()
{
	SUPER::Free();
}

void CMonsterBullet::OnCollision(CGameObject* pDst, const FContact* const)
{

}
void CMonsterBullet::OnCollisionEntered(CGameObject* pDst, const FContact* const)
{
	if(false == Attack_Occurrence(pDst, m_fAttack))
		m_pTransformComp->Move_Pos(&vDir, 0.001f, m_fMovingSpeed); // �浹�ߴµ� ����� �׾����� ��� ����
}

void CMonsterBullet::OnCollisionExited(CGameObject* pDst)
{
}
