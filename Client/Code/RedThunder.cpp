#include "stdafx.h"
#include "RedThunder.h"

CRedThunder::CRedThunder(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CRedThunder::CRedThunder(const CRedThunder& rhs)
	: Base(rhs)
{
}

CRedThunder::~CRedThunder()
{
}

CRedThunder* CRedThunder::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, 
	MonsterPhase _CurrPhase, CGameObject* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("RedThunder Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CRedThunder::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//�⺻���� 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"RedThunder");
	m_pTextureComp->Set_Scale({ 3.f, 12.f, 1.f });
	m_pTextureComp->Set_PosY(4.5f);
	m_pTextureComp->Readjust_Transform();
	m_fAttack = 6.f;

	// ������ �� ����ð� ����
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 6.f;
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 4.f;

	// �浹��
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pBoxShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
	pBoxShape->vHalfSize = { 1.f, 4.f, 0.3f };

	return S_OK;
}

_int CRedThunder::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	Billboard();

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = m_tFrame.fFrameEnd - 1.f;
		Set_Dead();
	}

	//Height_On_Terrain(6.f);

	//���� ������Ʈ �ڵ�
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CRedThunder::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CRedThunder::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));
	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

#pragma region �浹 �޽� �ݶ��̴�
	MeshBoxColider(_float(pBoxShape->vHalfSize.x), _float(pBoxShape->vHalfSize.y), _float(pBoxShape->vHalfSize.z));
#pragma endregion

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CRedThunder::Add_Component()
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
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER ); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CRedThunder::Free()
{
	SUPER::Free();
}

void CRedThunder::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"��RedThunder �浹 \n");

	Attack_Occurrence(pDst, m_fAttack);

}

void CRedThunder::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CRedThunder::OnCollisionExited(CGameObject* pDst)
{
}
