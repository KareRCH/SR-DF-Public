#include "stdafx.h"
#include "FallingStone.h"

CFallingStone::CFallingStone(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CFallingStone::CFallingStone(const CFallingStone& rhs)
	: Base(rhs)
{
}

CFallingStone::~CFallingStone()
{
}

CFallingStone* CFallingStone::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, MonsterPhase _pHASE, CAceUnit* pOwner, ETEAM_ID _eTeamid)
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

HRESULT CFallingStone::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// �̹��� ���� 
	int iCombo = (rand() % 2) + 1;
	if (1 == iCombo)
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Stone1");
	else
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Stone2");

	//�⺻���� 
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });
	m_fAttack = 3.f;				// ���ݷ� ���� 
	vFall = { 0.f, -1.f, 0.f };		// ���� ���� 
	m_fFallingSpeed = 6.f;			// ���ϼӵ� 

	// ������ ���� 
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameSpeed = 3.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 1.f;
	
	// �浹��
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pShape->fRadius = 0.5f;

	return S_OK;
}

_int CFallingStone::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard();

	Owner_Dead(m_pOwner);

	
	if(m_bFall)
		Falling(fTimeDelta); //���� 

	// �ȸ°� �׳� �����ؼ� ���� �浹 or ���𰡿� �浹 
	if (m_bCollision)
	{
		if (m_bSound)
		{
			Engine::Play_Sound(L"Enemy", L"_StoneBreak.wav", SOUND_ENEMY_EFFECT, 0.4f);
			m_bSound = FALSE;
		}

		m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
			Set_Dead();
		}
	}


	//���� ������Ʈ �ڵ�
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CFallingStone::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CFallingStone::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

#pragma region �⺻���� 

HRESULT CFallingStone::Add_Component()
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
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSS_SKILL); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER); // ��� �浹�ؾ��ϴ� ���̾�� 

	return S_OK;
}

void CFallingStone::Free()
{
	SUPER::Free();
}

void CFallingStone::Falling(const _float& fTimeDelta)
{
	if (m_pTransformComp->Get_Pos().y < 0.5f)
	{
		
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"FallingStone");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

		m_bCollision = TRUE;
	}
	else
		m_pTransformComp->Move_Pos(&vFall, fTimeDelta, m_fFallingSpeed);

}
#pragma endregion

void CFallingStone::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CFallingStone::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	OutputDebugString(L"��FallingStone �浹 \n");
	
	if(Attack_Occurrence(pDst, m_fAttack))
		m_bCollision = TRUE;
}

void CFallingStone::OnCollisionExited(CGameObject* pDst)
{
}
