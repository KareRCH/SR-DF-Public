#include "stdafx.h"
#include "SpawnFire.h"

#include "Player.h"

CSpawnFire::CSpawnFire(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CSpawnFire::CSpawnFire(const CSpawnFire& rhs)
	: Base(rhs)
{
}

CSpawnFire::~CSpawnFire()
{
}

CSpawnFire* CSpawnFire::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _CurrPhase, CAceUnit* pOwner, ETEAM_ID _eTeamid)
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

HRESULT CSpawnFire::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �⺻���� 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"FireEffect");
	m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });
	m_fAttack = 4.f;

	// ������ �� ����ð� ����
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 2.f;
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 5.f;

	// �浹��
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.2f;

	return S_OK;
}

_int CSpawnFire::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	Billboard();

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;
	m_tFrame.fAge += fTimeDelta * 1.f;

	//�Ϸ��̴� �뵵 ������ 
	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0;
	}

	if (m_tFrame.fAge > m_tFrame.fLifeTime)
		Set_Dead();


	//���� ������Ʈ �ڵ�
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

#pragma region �⺻���� 

void CSpawnFire::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CSpawnFire::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSpawnFire::Add_Component()
{
	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	// �� ����
	//m_pOwner = dynamic_cast


	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSS_SKILL); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CSpawnFire::Free()
{
	Engine::Stop_Sound(SOUND_ENEMY_FIRE);
	SUPER::Free();
}

#pragma endregion

#pragma region �浹 

void CSpawnFire::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CSpawnFire::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"��SpawnFire �浹 \n");

	if (Attack_Occurrence(pDst, m_fAttack))
		Set_Dead();
}

void CSpawnFire::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion

void CSpawnFire::Value_Setting(_float _x, _float _y, _float _z, MonsterPhase _phase)
{
	switch (_phase)
	{
	case Engine::Phase1:
		m_tFrame.fLifeTime = 5.f;
		break;

	case Engine::Phase2:
		m_tFrame.fLifeTime = 7.f;
		break;
	}
}
