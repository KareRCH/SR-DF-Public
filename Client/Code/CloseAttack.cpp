#include "stdafx.h"
#include "CloseAttack.h"
#include "Player.h"

CCloseAttack::CCloseAttack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerAttackUnion(pGraphicDev)
{
}

CCloseAttack::CCloseAttack(const CCloseAttack& rhs)
	: CPlayerAttackUnion(rhs)
{
}

CCloseAttack::~CCloseAttack()
{
}

HRESULT CCloseAttack::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �浹
	m_pTransformComp->Readjust_Transform();
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = m_tAttack.fSize;

	// �÷��̾ �ٶ󺸴� ������ ���ư�����
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// �߻� ���� ����
	m_tAttack.vDir = m_pPlayerTransformcomp->Get_Look();

	// �÷��̾� �޾ƿ���
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	m_bDbugFrame = pPlayer->Get_DBugFrame();

	// ������ ����
	m_tAttack.fDamage = 7.f;

	return S_OK;
}

_int CCloseAttack::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	//m_pTransformComp->Move_Pos(&m_tFist.vDir, fTimeDelta, m_tFist.fMoveSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 

	// �ð� ������Ʈ
	m_tAttack.fCurTime += 1.f * fTimeDelta;

	if (m_tAttack.fCurTime >= m_tAttack.fDeleteTime)
	{
		Set_Dead(); // ���� ����
		m_tAttack.fCurTime = 0.f;
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CCloseAttack::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CCloseAttack::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	if (*m_bDbugFrame)
	{
	//	MeshSphereColider(_float(pShape->fRadius), 30, 30);
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CCloseAttack* CCloseAttack::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir, CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID, _float fMoveSpeed, _float fDeleteTime, _float fDamage, _float fSize, STATE_RIGHTHAND _pType)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("PlayerBullet Create Failed");
		return nullptr;
	}

	_float fMovePos = 1.2f;
	_vec3 vAttackPosition = vPos + vDir * fMovePos;

	pInstance->m_pTransformComp->Set_Pos(vAttackPosition.x, vAttackPosition.y, vAttackPosition.z);	// ���� ��ġ
	pInstance->Set_PlayerAttackState(vDir, _AttackState, fMoveSpeed, fDeleteTime, fDamage, fSize);	// ���� ����
	pInstance->Set_Owner(_Owner);									// ������ ����
	pInstance->Set_TeamID(_eTeamID);	// ������ �� ����
	pInstance->m_pWeaponType = _pType; //������������

	return pInstance;
}

HRESULT CCloseAttack::Add_Component()
{
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);


	// -------------------- �浹 ��Ʈ --------------------------
	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER_ATTACK); // �� Ŭ������ ���� �浹���̾� (�÷��̾� ����)
	m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER); // ��� �浹�ؾ��ϴ� ���̾�� (����)
	m_pColliderComp->Get_CollisionMask();
	return S_OK;
}

void CCloseAttack::Free()
{
	SUPER::Free();
}

void CCloseAttack::OnCollision(CGameObject* pDst, const FContact* const pContact)
{


}

void CCloseAttack::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"�÷��̾��� �ָ� �浹 \n");
	
	// ���� ����  (������, �� ������ ���� Ÿ��, �� ������ ����)
	Change_MonsterHp(-m_tAttack.fDamage, pDst, m_tAttack.ePlayer_AttackState, m_pWeaponType);
	
	// Test �ɱ� ���� Ȯ��
	if (m_tAttack.ePlayer_AttackState == PSITDONW_ATTACK)
	{
		OutputDebugString(L"�÷��̾ �ɾƼ� ������ \n");
	}

	// �Ѿ� ����
	Set_Dead(); //����ü�� ����� 
}

void CCloseAttack::OnCollisionExited(CGameObject* pDst)
{
}
