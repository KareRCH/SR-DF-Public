#include "stdafx.h"
#include "Stage.h"
#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "BackGround.h"
#include "SkyBox.h"
#include "UI_Player.h"
#include <AceBuilding.h>
#include "UI_Hagrid.h"


CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage::~CStage()
{

}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CStage::Free()
{
	SUPER::Free();
}

HRESULT CStage::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Camera(L"Camera"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);

	//Engine::StopAll();

	return S_OK;
}

_int CStage::Update_Scene(const _float & fTimeDelta)
{
	_int	iExit = SUPER::Update_Scene(fTimeDelta);

	return iExit;
}

void CStage::LateUpdate_Scene()
{
	SUPER::LateUpdate_Scene();
}

void CStage::Render_Scene()
{
	// _DEBUG 용

}

HRESULT CStage::ReadyLate_Scene()
{
	

	return S_OK;
}

HRESULT CStage::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(-10.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ pLayerTag, pLayer });

	Engine::CGameObject*		pGameObject = nullptr;

	// SkyBox
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", CSkyBox::Create(m_pGraphicDev, 10.f)), E_FAIL);

	// Particle
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SnowParticle", CSnowParticle::Create(m_pGraphicDev, {40.f, 3.f, 40.f}, 10000) ), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FireWorkParticle", CFireWork::Create(m_pGraphicDev, { 24.f, 3.f, 12.f }, 200)), E_FAIL);
	
	pLayer = Engine::CLayer::Create(-10.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ L"Terrain", pLayer});

	// Terrain
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", CTerrain::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(0.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ pLayerTag, pLayer });

	Engine::CGameObject*		pGameObject = nullptr;

	// NPC
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Hagrid",  CHagrid::Create(m_pGraphicDev, 25.f, 2.2f, 30.f)), E_FAIL);

	// Player
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", CPlayer::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"PlayerLighter", CPlayerLighter::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"PlayerGunLighter", CPlayerGunLighter::Create(m_pGraphicDev)), E_FAIL);

	// Bullet
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Bullet", CBullet::Create(m_pGraphicDev)), E_FAIL);

	// Monster - TEST중이라 상시로 서로 위치 바뀝니다. 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Boss", CAceBoss::Create(m_pGraphicDev, 65.f, 1.f, 25.f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray2",  CGray::Create(m_pGraphicDev, 15.f, 1.f, 50.f)), E_FAIL);
	
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray4",  CBrown::Create(m_pGraphicDev, 15.f, 1.f, 20.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray5",  CBrown::Create(m_pGraphicDev, 15.75f, 1.f, 22.5f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray6",  CBrown::Create(m_pGraphicDev, 17.5f, 1.f, 24.25f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray7",  CBrown::Create(m_pGraphicDev, 20.f, 1.f, 25.f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray8",  CBrown::Create(m_pGraphicDev, 22.5f, 1.f, 24.25f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Gray",   CBrown::Create(m_pGraphicDev, 24.25f, 1.f, 22.5f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Brown3", CBrown::Create(m_pGraphicDev, 25.f, 1.f, 20.f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Brown",  CBrown::Create(m_pGraphicDev, 24.25f, 1.f, 17.5f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Brown2", CBrown::Create(m_pGraphicDev, 22.5f, 1.f, 15.75f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Brown21", CBrown::Create(m_pGraphicDev, 20.f, 1.f, 15.f)), E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Brown22", CBrown::Create(m_pGraphicDev, 17.5f, 1.f, 15.75f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Brown23", CBrown::Create(m_pGraphicDev, 15.75f, 1.f, 17.5f)), E_FAIL);

	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Boss", CBoss::Create(m_pGraphicDev, 60.f, 1.f, 25.f)), E_FAIL);

	// 스킬 실험실
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Barrier", CBarrier::Create(m_pGraphicDev, 5.f, 1.f, 15.f)), E_FAIL);


	// =========================================

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Building1", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::BUILDING, L"A", 14.f, 1.f, 100.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Building2", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::BUILDING, L"B", 24.f, 1.f, 100.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Building3", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::BUILDING, L"C", 34.f, 1.f, 100.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Building4", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::BUILDING, L"D", 44.f, 1.f, 100.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Building5", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::BUILDING, L"F", 64.f, 1.f, 100.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Building6", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::BUILDING, L"E", 80.f, 1.f, 100.f)), E_FAIL);

	// Object - 사이즈 조절이 필요해서 하나씩 다 stage에 올려봐야함 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food1", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"EatenApple", 24.f, 1.f, 25.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food2", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"BananaPeel", 32.f, 1.f, 25.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food3", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"Banana", 28.f, 1.f, 25.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food4", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"Medkit", 40.f, 1.f, 25.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food5", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"Apple", 20.f, 1.f, 25.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food6", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"Cola", 36.f, 1.f, 25.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Food7", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::FOOD, L"Cola", 44.f, 1.f, 25.f)), E_FAIL);
	
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Weapon8", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::WEAPON, L"FryingPan", 20.f, 1.f, 20.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Weapon9", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::WEAPON, L"TommyGun", 22.f, 1.f, 20.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Weapon11", CAceObjectFactory::Create(m_pGraphicDev,OBJECT_CLASS::WEAPON, L"Pistol", 26.f, 1.f, 20.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Weapon13", CAceObjectFactory::Create(m_pGraphicDev,OBJECT_CLASS::WEAPON, L"Bottle", 28.f, 1.f, 20.f)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Weapon12", CAceObjectFactory::Create(m_pGraphicDev,OBJECT_CLASS::WEAPON, L"Pipe", 30.f, 1.f, 20.f)), E_FAIL);
	
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction1", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"BLOCK_SWITCH", 24.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction2", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"LIGHT_SWITCH", 28.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction3", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"Building_Key", 52.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction4", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"Bossroom_Key", 56.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction5", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"TwoButton", 32.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction6", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"OneButton", 36.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction7", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"Newspaper", 44.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction8", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"ComicBook", 48.f, 1.f, 16.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction9", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::INTERACTION, L"Padlock", 40.f, 1.f, 16.f)), E_FAIL);
	
	// 너무많아서 그냥 이정도만 깔음 - 스프라이트 형식으로 넣어서 Cnt값의 Frame을 넣어주면 해당하는거 나옴 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw3", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::THROW, L"Lamp_0", 20.f, 1.f, 12.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw", CAceObjectFactory::Create(m_pGraphicDev,  OBJECT_CLASS::THROW, L"BeerGlass", 22.f, 1.f, 12.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw1", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::THROW, L"CafeTable", 24.f, 1.f, 12.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw2", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::THROW, L"CafeTable", 26.f, 1.f, 12.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw4", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::THROW, L"Trashbin_0", 28.f, 1.f, 12.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw6", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::THROW, L"PaintCan_2", 30.f, 1.f, 12.f)), E_FAIL); 
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Throw5", CAceObjectFactory::Create(m_pGraphicDev, OBJECT_CLASS::THROW, L"WoodenStool", 32.f, 1.f, 12.f)), E_FAIL); 
	
	// FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Interaction1", CAceObjectFactory::Create(m_pGraphicDev, CAceObjectFactory::OBJECT_CLASS::INTERACTION, L"BLOCK_SWITCH", 24.f, 1.f, 12.f)), E_FAIL); //
	// Add_GameObject에 들어가는 pObjTag 가 다 달라야 들어감 -> Object 피킹하는데 연산량이 너무 증가함
	// 해결방안 생각해봐야 할듯 

	return S_OK;
}

HRESULT CStage::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create(-1.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ pLayerTag, pLayer });

	Engine::CGameObject* pGameObject = nullptr;

	// DynamicCamera
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 5.f, -10.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f))), E_FAIL);


	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(-100.f);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Player", CUI_Player::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_CrossHair", CUI_CrossHair::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_PlayerFace", CUI_PlayerFace::Create(m_pGraphicDev)), E_FAIL);

	
	m_mapLayer.insert({ pLayerTag, pLayer });
	
	return S_OK;
}

HRESULT CStage::Ready_Layer_Completed()
{
	
	return S_OK;
}


	