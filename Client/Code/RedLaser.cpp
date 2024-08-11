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
	// 생성할때 몬스터 위치 로 생성하기 위해 Create에서 초기위치를 잡아줌 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CRedLaser::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 기본설정
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Razer"); //마우스는 역시 Razer
	m_pTransformComp->Set_Scale({ 10.f, 2.f, 1.f });
	m_fAttack = 4.f;

	// 프레임 설정 
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 10.f;
	m_tFrame.fFrame = 0.f;
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 2.f;

	// 충돌용 - BOX
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
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

	//물리 업데이트 코드
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 콜라이더 위치 업데이트 

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
	// 콜라이더 컴포넌트
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);

	// 물리 세계 등록
	m_pColliderComp->EnterToPhysics(0);

	// 충돌 함수 연결
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// 충돌 레이어, 마스크 설정
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSS_SKILL); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

	return S_OK;
}

void CRedLaser::Free()
{
	SUPER::Free();
}

void CRedLaser::OnCollision(CGameObject* pDst, const FContact* const pContact) 
{
	// 맞고있는게 계속있으면 2초에 한번씩 피 닳을것임 
	if (m_tFrame.fAge >= m_tFrame.fLifeTime)
	{
		m_tFrame.fAge = 0.f;
		Attack_Occurrence(pDst, m_fAttack);
	}
}

void CRedLaser::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{

	OutputDebugString(L"▶RedLaser 충돌 \n");

	if(Attack_Occurrence(pDst, m_fAttack))
			KnockBack = TRUE;
}

void CRedLaser::OnCollisionExited(CGameObject* pDst)
{
	KnockBack = FALSE;
}
