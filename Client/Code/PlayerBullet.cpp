#include "stdafx.h"

#include "Player.h"
#include "PlayerBullet.h"
#include "Effect_HitPow.h"

CPlayerBullet::CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerAttackUnion(pGraphicDev)
{
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& rhs)
	: CPlayerAttackUnion(rhs)
{
}

CPlayerBullet::~CPlayerBullet()
{

}

HRESULT CPlayerBullet::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	Set_TeamID(ETEAM_PLAYER);

	// �浹
	m_pTransformComp->Readjust_Transform();
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = m_tAttack.fSize;

	// �÷��̾ �ٶ󺸴� ������ ���ư�����
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// �߻� ���� ����
	m_tAttack.vDir = m_pPlayerTransformcomp->Get_Look();
	m_tAttack.fMoveSpeed = 5.f;
	// �÷��̾� �޾ƿ���
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
	m_bDbugFrame = pPlayer->Get_DBugFrame();

	// ������ ����
	//m_tAttack.fDamage = 55.f;

	return S_OK;
}

_int CPlayerBullet::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);
	
	m_pTransformComp->Move_Pos(&m_tAttack.vDir, fTimeDelta, m_tAttack.fMoveSpeed);

	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	// �ð� ������Ʈ
	m_tAttack.fCurTime += 1.f * fTimeDelta;

	if (m_tAttack.fCurTime >= m_tAttack.fDeleteTime)
	{
		m_vPos = m_pTransformComp->Get_Pos();
		//CEffect_HitPow::Create(m_pGraphicDev, m_vPos.x, m_vPos.y, m_vPos.z, this);

		Set_Dead(); // ���� ����
		m_tAttack.fCurTime = 0.f;
	}

	return S_OK;
}

void CPlayerBullet::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CPlayerBullet::Render_GameObject()
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

CPlayerBullet* CPlayerBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
	_vec3 vPos, _vec3 vDir, CAceUnit* _Owner, 
	PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID, 
	_float fMoveSpeed, _float fDeleteTime, _float fDamage, 
	_float fSize, STATE_RIGHTHAND pRight)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("PlayerBullet Create Failed");
		return nullptr;
	}

	// �÷��̾� ��ġ���� ����
	pInstance->m_pTransformComp->Set_Pos(vPos.x, vPos.y + 0.7f, vPos.z);	// ���� ��ġ
	pInstance->Set_PlayerAttackState(vDir, _AttackState, fMoveSpeed, fDeleteTime, fDamage, fSize); // ���� ���� ����
	pInstance->Set_Owner(_Owner);									// ������ ����
	pInstance->Set_TeamID(_eTeamID);	// ������ �� ����
	pInstance->m_pWeaponType = pRight; // �������� ���� 

	return pInstance;
}

HRESULT CPlayerBullet::Add_Component()
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
	m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� (����)
	return S_OK;
}

void CPlayerBullet::Free()
{
	SUPER::Free();
}

void CPlayerBullet::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
	
}

void CPlayerBullet::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

	if (pAceObj == nullptr)
		return;

	if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
	{
		if( false == Change_MonsterHp(-m_tAttack.fDamage, pDst, m_tAttack.ePlayer_AttackState, m_pWeaponType))
			m_pTransformComp->Move_Pos(&m_tAttack.vDir, 0.001f, m_tAttack.fMoveSpeed);
		else
			Set_Dead(); //����ü�� ����� 
		
	}

	//pContact->pBody
}

void CPlayerBullet::OnCollisionExited(CGameObject* pDst)
{
}
