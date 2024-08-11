#include "stdafx.h"
#include "ThrowPipe.h"

CThrowPipe::CThrowPipe(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CThrowPipe::CThrowPipe(const CThrowPipe& rhs)
	: Base(rhs)
{
}

CThrowPipe::~CThrowPipe()
{
}

CThrowPipe* CThrowPipe::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("ThrowPipe Create Failed");
		return nullptr;
	}

	// 생성할때 몬스터 위치 로 생성하기 위해 Create에서 초기위치를 잡아줌 
	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}


HRESULT CThrowPipe::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 셋팅
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Projectile", L"ThrowPipe");
	m_pTransformComp->Set_Scale({ 0.3f, 0.7f, 1.f });
	m_fMoveSpeed = 7.f;
	m_fAttack = 5.f;
	_bLoop = FALSE;

	//변수 값 조정
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 10.f;
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 4.f;

	// 충돌
	m_pTransformComp->Readjust_Transform();
	pSphereShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
	pSphereShape->fRadius = 0.3f;


	return S_OK;
}

_int CThrowPipe::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);
	
	Owner_Dead(m_pOwner);

	if (!_bLoop) // 한번 정한 방향으로만 날라가서 플레이어가 다 피할수있도록 
		Dir_Setting();

	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;
	m_tFrame.fAge += 1.f * fTimeDelta;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0.f;
	}

	if (m_tFrame.fAge > m_tFrame.fLifeTime)
		Set_Dead();

	m_pTransformComp->Move_Pos(&m_vDir, fTimeDelta, m_fMoveSpeed);
	
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CThrowPipe::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CThrowPipe::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

#pragma region 충돌 메쉬 콜라이더
	//MeshSphereColider(_float(pSphereShape->fRadius), 8, 16);
#pragma endregion

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
}

HRESULT CThrowPipe::Add_Component()
{
	// 콜라이더 컴포넌트
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
	// 물리 세계 등록
	m_pColliderComp->EnterToPhysics(0);

	// 충돌 함수 연결
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// 충돌 레이어, 마스크 설정
	m_pColliderComp->Set_CollisionLayer(LAYER_PROJECTILE); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_WALL); // 얘랑 충돌해야하는 레이어들 

	return S_OK;
}

void CThrowPipe::Dir_Setting()
{
	// 플레이어 쪽으로 날아가기위해 방향벡터 잡기 
	m_vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&m_vDir, &m_vDir); //정규화 해서 스피드로 제어

	_bLoop = true;
}

void CThrowPipe::Free()
{
	SUPER::Free();
}

void CThrowPipe::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CThrowPipe::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
	//OutputDebugString(L"Gray의 투사체와 충돌시작 \n");

	if(Attack_Occurrence(pDst, m_fAttack))
		Set_Dead(); //투사체는 사라짐 
}

void CThrowPipe::OnCollisionExited(CGameObject* pDst)
{
}
