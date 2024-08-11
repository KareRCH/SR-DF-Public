#include "stdafx.h"
#include "PlayerLightning.h"
#include "Player.h"
#include "Brown.h"
#include "Gray.h"
#include "AceBoss.h"

CPlayerLightning::CPlayerLightning(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerSkillUnion(pGraphicDev)
{
}

CPlayerLightning::CPlayerLightning(const CPlayerLightning& rhs)
	: CPlayerSkillUnion(rhs)
{
}

CPlayerLightning::~CPlayerLightning()
{

}

HRESULT CPlayerLightning::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	Set_TeamID(ETEAM_PLAYER);

	Monster_Select(m_pGameObject);

	// ����Ʈ ���ҽ�(�̹���) �ޱ�
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Explosion");
	//m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Lightning");
	// �̹��� ������
	m_pTransformComp->Set_Scale({ 4.f, 4.f, 4.f });
	// �̹��� ���� ��ġ
	//m_pTransformComp->Set_Pos(pMonster->Get_MonsterPos());

	m_vPos = Target_Pos(m_pGameObject);
	//vTargetPos.y += 1.f;
	m_vPos.y += 0.3f;
	//m_pTransformComp->Set_Pos(vTargetPos);

	// ����Ʈ ������
	m_tSkill.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	//m_tSkill.fFrameSpeed = 6.f;
	m_tSkill.fFrameSpeed = 30.f;
	m_tSkill.fFrame = 0;

	// �浹
	m_pTransformComp->Readjust_Transform();

	// �÷��̾ �ٶ󺸴� ��
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	return S_OK;
}

_int CPlayerLightning::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard(); // ������

	// �ð� ������Ʈ
	m_tSkill.fFrame += m_tSkill.fFrameSpeed * fTimeDelta;

	m_pTransformComp->Set_Pos(m_vPos);

	if (!m_bSoundOff) // ���� �ѹ��� ���
	{
		// ���ݸ��� ���� ��� (���� ���߰���, ���� ����)
		if (true)
		{
			// ���� ex)
			// �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
			Engine::Play_Sound(L"FallenAces", L"Explosion (0).wav", SOUND_PLAYER_EFFECT, m_fVolume);

		}
		else if (true)
		{
			// ���� ex)
			// �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
			Engine::Play_Sound(L"FallenAces", L"Explosion (1).wav", SOUND_PLAYER_EFFECT, m_fVolume);

		}
		else if (true)
		{
			// ���� ex)
			// �ѹ��� ���(L"��θ� �˰��ִ� Ű��", L"���ϸ�.Ȯ����", ���� ä��, ����);
			Engine::Play_Sound(L"FallenAces", L"Explosion (2).wav", SOUND_PLAYER_EFFECT, m_fVolume);

		}

		m_bSoundOff = true; // ���� ��� ����
	}


	// Ÿ��
	if (m_tSkill.fFrame > 8.f &&
		m_ePlayer_SkillHit == PSKILLHIT_NOHIT)
	{
		Change_MonsterHp(-200.f, m_pGameObject, PNOMAL_ATTACK);
		m_ePlayer_SkillHit = PSKILLHIT_ONE;
	}

	if (m_tSkill.fFrame >= m_tSkill.fFrameEnd)
	{
		m_tSkill.fFrame = m_tSkill.fFrameEnd - 1.f;

		Set_Dead();
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CPlayerLightning::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CPlayerLightning::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tSkill.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CPlayerLightning* CPlayerLightning::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _pGameObject)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	//pInstance->Monster_Select(_pAceMonster);

	pInstance->Set_GameObject(_pGameObject);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("PlayerBullet Create Failed");
		return nullptr;
	}


	return pInstance;
}

HRESULT CPlayerLightning::Add_Component()
{
	//// -------------------- �浹 ��Ʈ --------------------------
	//// �ݶ��̴� ������Ʈ
	//NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	//// ���� ���� ���
	//m_pColliderComp->EnterToPhysics(0);

	//// �浹 �Լ� ����
	//m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	//m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	//m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	//// �浹 ���̾�, ����ũ ����
	//m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER_ATTACK); // �� Ŭ������ ���� �浹���̾� (�÷��̾� ����)
	//m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER); // ��� �浹�ؾ��ϴ� ���̾�� (����)
	return S_OK;
}

void CPlayerLightning::Free()
{
	SUPER::Free();
}

void CPlayerLightning::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CPlayerLightning::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

	if (pAceObj == nullptr)
		return;

	if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE)
	{
		// ���� ����  (������, �� ������ ���� Ÿ��, �� ������ ����)
		//Change_MonsterHp(-m_tAttack.fDamage, pDst, m_tAttack.ePlayer_AttackState);

		//// Test ���� Ȯ��
		//if (m_tAttack.ePlayer_AttackState == PSITDONW_ATTACK)
		//{
		//	OutputDebugString(L"�÷��̾ �ɾƼ� ������ \n");
		//}
		//else
		//{
		//	OutputDebugString(L"�÷��̾��� �Ѿ� �浹 \n");
		//}

		// �Ѿ� ����
		Set_Dead(); //����ü�� ����� 
	}
}

void CPlayerLightning::OnCollisionExited(CGameObject* pDst)
{
}
