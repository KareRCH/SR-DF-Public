#include "stdafx.h"
#include "RedLaser.h"

#include "Player.h"

CRedLaser::CRedLaser(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CRedLaser::CRedLaser(const CRedLaser& rhs)
	: Base(rhs)
{
}

CRedLaser::~CRedLaser()
{
}

CRedLaser* CRedLaser::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAceUnit* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("LightBeam Create Failed");
		return nullptr;
	}
	// �����Ҷ� ���� ��ġ �� �����ϱ� ���� Create���� �ʱ���ġ�� ����� 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CRedLaser::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �⺻����
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Razer"); //���콺�� ���� Razer
	m_pTransformComp->Set_Scale({ 10.f, 2.f, 1.f });
	m_fAttack = 4.f;

	// ������ ���� 
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 10.f;
	m_tFrame.fFrame = 0.f;
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 2.f;

	// �浹�� - BOX
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
	pShape->vHalfSize = { 5.f, 0.7f, 0.3f };

	return S_OK;
}

_int CRedLaser::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	if (KnockBack)
	{
		m_tFrame.fAge += 1.f * fTimeDelta;
		Knockback_Player(fTimeDelta, 6.f);
	}

	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0.f;
	}

	//���� ������Ʈ �ڵ�
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

	Engine::Add_RenderGroup(RENDER_ALPHATEST , this);

	return S_OK;
}

void CRedLaser::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CRedLaser::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CRedLaser::Add_Component()
{
	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);

	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSS_SKILL); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

	return S_OK;
}

void CRedLaser::Free()
{
	SUPER::Free();
}

void CRedLaser::OnCollision(CGameObject* pDst, const FContact* const pContact) 
{
	// �°��ִ°� ��������� 2�ʿ� �ѹ��� �� �������� 
	if (m_tFrame.fAge >= m_tFrame.fLifeTime)
	{
		m_tFrame.fAge = 0.f;
		Attack_Occurrence(pDst, m_fAttack);
	}
}

void CRedLaser::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{

	OutputDebugString(L"��RedLaser �浹 \n");

	if(Attack_Occurrence(pDst, m_fAttack))
			KnockBack = TRUE;
}

void CRedLaser::OnCollisionExited(CGameObject* pDst)
{
	KnockBack = FALSE;
}
