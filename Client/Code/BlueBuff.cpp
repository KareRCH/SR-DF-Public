#include "stdafx.h"
#include "BlueBuff.h"

CBlueBuff::CBlueBuff(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBlueBuff::CBlueBuff(const CBlueBuff& rhs)
	: Base(rhs)
{
}

CBlueBuff::~CBlueBuff()
{
}

CBlueBuff* CBlueBuff::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, 
	MonsterPhase _CurrPhase, CGameObject* pOwner, ETEAM_ID _eTeamid)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BlueBuff Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
	pInstance->Set_Owner(pOwner);
	pInstance->Set_TeamID(_eTeamid);

	return pInstance;
}

HRESULT CBlueBuff::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 가본셋팅 
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Buff");
	m_pTransformComp->Set_Scale({ 2.f, 2.f, 1.f });

	// 프레임 및 사망시간 조정
	m_tFrame.fFrame = 0;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 8.f;

	return S_OK;
}

_int CBlueBuff::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Owner_Dead(m_pOwner);

	Billboard();

	CTransformComponent* m_pBossTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Boss", L"Com_Transform"));
	m_pTransformComp->Set_Pos(m_pBossTransformcomp->Get_Pos());

	m_tFrame.fFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		//Engine::Play_Sound(L"Enemy", L"_Buff.wav", SOUND_ENEMY_BUFF, 0.2f);

		m_tFrame.fFrame = 0;
	}

	if (m_pOwner->Get_IsDead()) // 보스가 죽으면 버프도 사라짐 
		Set_Dead();


	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

#pragma region 기본셋팅 

void CBlueBuff::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CBlueBuff::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBlueBuff::Add_Component()
{
	//얘는 충돌 없음 - 나중대비 일단 함수는 넣어둠 
	return S_OK;
}

void CBlueBuff::Free()
{
	//Engine::Stop_Sound(SOUND_ENEMY_BUFF);
	SUPER::Free();
}

void CBlueBuff::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CBlueBuff::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CBlueBuff::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion