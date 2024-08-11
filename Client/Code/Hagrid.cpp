#include "stdafx.h"
#include "Hagrid.h"
#include "Player.h"
#include "UI_Hagrid.h"

void CHagrid::Free()
{
	SUPER::Free();
}

CHagrid::CHagrid(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CHagrid::CHagrid(const CHagrid& rhs)
	: Base(rhs)
{
}

CHagrid::~CHagrid()
{
}

HRESULT CHagrid::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//Add_Component();

	// 이미지 및 프레임 셋팅
	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"NPC", L"Hagrid");
	m_pTextureComp->Readjust_Transform();

	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 2.5f;

	return S_OK;
}

HRESULT CHagrid::Ready_GameObject(const FSerialize_GameObject tObjectSerial)
{
	FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

	m_pTransformComp->Set_Pos(tObjectSerial.vPos);
	m_pTransformComp->Set_Rotation(tObjectSerial.vRotation);
	m_pTransformComp->Set_Scale(tObjectSerial.vScale);

	wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
	Set_ObjectName(strConvName);

	m_fPriority[0] = tObjectSerial.fPriority_Update;
	m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
	m_fPriority[2] = tObjectSerial.fPriority_Render;

	m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
	m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
	m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

	if (!tObjectSerial.strGroupKey.empty() && !tObjectSerial.strTextureKey.empty())
		m_pTextureComp->Receive_Texture(TEX_NORMAL,
		wstring(tObjectSerial.strGroupKey.begin(), tObjectSerial.strGroupKey.end()).c_str(),
		wstring(tObjectSerial.strTextureKey.begin(), tObjectSerial.strTextureKey.end()).c_str());


	return S_OK;
}

_int CHagrid::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	Billboard(fTimeDelta);

	m_tFrame.fAge += fTimeDelta;

	if (m_tFrame.fAge > m_tFrame.fLifeTime && !m_bOnce)
	{
		Engine::Play_Sound(L"FallenAces", L"yourwizard.wav", SOUND_EFFECT, 0.8f);

		Engine::Add_GameObject(L"GameLogic", CUI_Hagrid::Create(m_pGraphicDev));
		m_bOnce = true;
	}

	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CHagrid::LateUpdate_GameObject()
{
}

void CHagrid::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(0);
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CHagrid* CHagrid::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, _vec3 _size)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Hegrid Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z); // 시작위치 설정
	pInstance->m_pTransformComp->Set_Scale(_size);
	return pInstance;
}
CHagrid* CHagrid::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
	{
		Safe_Release(pInstance);

		MSG_BOX("npc Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CHagrid::Add_Component()
{
	// 버퍼 컴포넌트
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);

	return S_OK;
}

void CHagrid::Height_On_Terrain()
{
	_vec3		vPos;
	m_pTransformComp->Get_Info(INFO_POS, &vPos);

	CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
	NULL_CHECK(pTerrainBufferComp);

	_float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
		pTerrainBufferComp->Get_VtxPos(),
		pTerrainBufferComp->Get_VertexCountX() + 1U,
		pTerrainBufferComp->Get_VertexCountZ() + 1U,
		pTerrainBufferComp->Get_Scale(),
		pTerrainBufferComp->Get_InvOffset());

	m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.5f, vPos.z);
}

HRESULT CHagrid::Billboard(const _float& fTimeDelta)
{
	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

	// 플레이어를 바라보게 함
	_vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

	D3DXVec3Normalize(&vDir, &vDir);

	_float rad = atan2f(vDir.x, vDir.z);

	m_pTransformComp->Set_RotationY(rad);

	return S_OK;
}

//HRESULT CHagrid::Get_PlayerPos()
//{
//	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
//	m_vPos = m_pTransformComp->Get_Pos();
//
//	m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
//	NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);
//
//
//	return S_OK;
//}