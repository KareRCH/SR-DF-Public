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

	// 이펙트 리소스(이미지) 받기
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Explosion");
	//m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Lightning");
	// 이미지 사이즈
	m_pTransformComp->Set_Scale({ 4.f, 4.f, 4.f });
	// 이미지 생성 위치
	//m_pTransformComp->Set_Pos(pMonster->Get_MonsterPos());

	m_vPos = Target_Pos(m_pGameObject);
	//vTargetPos.y += 1.f;
	m_vPos.y += 0.3f;
	//m_pTransformComp->Set_Pos(vTargetPos);

	// 이펙트 프레임
	m_tSkill.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	//m_tSkill.fFrameSpeed = 6.f;
	m_tSkill.fFrameSpeed = 30.f;
	m_tSkill.fFrame = 0;

	// 충돌
	m_pTransformComp->Readjust_Transform();

	// 플레이어가 바라보는 쪽
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	return S_OK;
}

_int CPlayerLightning::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard(); // 빌보드

	// 시간 업데이트
	m_tSkill.fFrame += m_tSkill.fFrameSpeed * fTimeDelta;

	m_pTransformComp->Set_Pos(m_vPos);

	if (!m_bSoundOff) // 사운드 한번만 재생
	{
		// 공격마다 랜덤 재생 (현재 폭발공격, 폭발 사운드)
		if (true)
		{
			// 사운드 ex)
			// 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
			Engine::Play_Sound(L"FallenAces", L"Explosion (0).wav", SOUND_PLAYER_EFFECT, m_fVolume);

		}
		else if (true)
		{
			// 사운드 ex)
			// 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
			Engine::Play_Sound(L"FallenAces", L"Explosion (1).wav", SOUND_PLAYER_EFFECT, m_fVolume);

		}
		else if (true)
		{
			// 사운드 ex)
			// 한번만 재생(L"경로를 알고있는 키값", L"파일명.확장자", 사운드 채널, 볼륨);
			Engine::Play_Sound(L"FallenAces", L"Explosion (2).wav", SOUND_PLAYER_EFFECT, m_fVolume);

		}

		m_bSoundOff = true; // 사운드 재생 끄기
	}


	// 타격
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
	//// -------------------- 충돌 세트 --------------------------
	//// 콜라이더 컴포넌트
	//NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

	//// 물리 세계 등록
	//m_pColliderComp->EnterToPhysics(0);

	//// 충돌 함수 연결
	//m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	//m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	//m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	//// 충돌 레이어, 마스크 설정
	//m_pColliderComp->Set_CollisionLayer(LAYER_PLAYER_ATTACK); // 이 클래스가 속할 충돌레이어 (플레이어 공격)
	//m_pColliderComp->Set_CollisionMask(LAYER_MONSTER | LAYER_BOSSMONSTER); // 얘랑 충돌해야하는 레이어들 (몬스터)
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
		// 몬스터 피해  (데미지, 이 공격을 받은 타겟, 이 공격의 유형)
		//Change_MonsterHp(-m_tAttack.fDamage, pDst, m_tAttack.ePlayer_AttackState);

		//// Test 공격 확인
		//if (m_tAttack.ePlayer_AttackState == PSITDONW_ATTACK)
		//{
		//	OutputDebugString(L"플레이어가 앉아서 공격함 \n");
		//}
		//else
		//{
		//	OutputDebugString(L"플레이어의 총알 충돌 \n");
		//}

		// 총알 삭제
		Set_Dead(); //투사체는 사라짐 
	}
}

void CPlayerLightning::OnCollisionExited(CGameObject* pDst)
{
}
