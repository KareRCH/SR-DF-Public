#include "stdafx.h"
#include "Header/Loading.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include <mutex>

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Loading Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_Crt);
	CloseHandle(m_hThread);

	Safe_Release(m_pGraphicDev);
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	m_eLoadingID = eID;

	m_iFutureLimit = thread::hardware_concurrency()*2;

	return S_OK;
}

_uint CLoading::Loading_For_Stage()
{
	FAILED_CHECK_RETURN(Loading_For_Texture(), E_FAIL);
	//사운드 로드하기
	FAILED_CHECK_RETURN(Engine::Ready_SoundDev(), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MeshColBufferComp", CMeshColComp::Create(m_pGraphicDev)), E_FAIL);

	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColBufferComp", CTriColorComp::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeBufferComp", CCubeBufferComp::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorComp", CCalculatorComponent::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainBufferComp", CTerrainBufferComp::Create(m_pGraphicDev, L"./Resource/Texture/Terrain/FlatLand.bmp")), E_FAIL);

	// 텍스처 컴포넌트
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Tile", L"22")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_CUBE, L"TestCube", L"4")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"HudBox_Main")), E_FAIL);
	
	// 플레이어 왼손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Left_RunHand")), E_FAIL);
	
	// 플레이어 오른손 + 플레이어 스킬 쓰는중
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Right_RunHand")), E_FAIL);

	// 몬스터 + 이펙트 + 오브젝트 + 파티클 돌려쓰는중 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonsterTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Boss_Single", L"CrotchHit")), E_FAIL);
	
	// 충돌 컴포넌트
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderSphereComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::SPHERE)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderBoxComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::BOX)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderCapsuleComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::CAPSULE)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderOBBComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::OBB)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderLineComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::LINE)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderRayComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::RAY)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ColliderTriangleComp", CColliderComponent::Create(m_pGraphicDev, ECOLLISION::TRIANGLE)), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);

	m_bFinish = true;

	return S_OK;
}

// 텍스처 매니저 텍스처 추가
HRESULT CLoading::Loading_For_Texture()
{

#pragma region Terrain, Default
	Load_Texture(L"./Resource/Texture/Tile/Tile/22.jpg", TEX_NORMAL, L"Tile", L"22");
	Load_Texture(L"./Resource/Texture/SkyBox/Test4.dds", TEX_CUBE, L"TestCube", L"4");
	//Load_Texture(L"./Resource/Texture/SkyBox/Skybox.dds", TEX_CUBE, L"TestCube", L"4");
#pragma endregion

#pragma region 싱글 텍스처
	// 왼손 달리기
	Load_Texture(L"./Resource/Texture/Player/RunHand/LeftHand_Run (%d).png", TEX_NORMAL, L"Player", L"Left_RunHand", _range<_uint>(0U, 0U));
	// 오른손 달리기
	Load_Texture(L"./Resource/Texture/Player/RunHand/RightHand_Run (%d).png", TEX_NORMAL, L"Player", L"Right_RunHand", _range<_uint>(0U, 0U));
	// 오픈 핸드 [오른손이 무언가를 할 때 아무것도 아닌상태] (왼손)
	Load_Texture(L"./Resource/Texture/Player/OpenHand/OpenHand (%d).png", TEX_NORMAL, L"Player", L"OpenHand", _range<_uint>(0U, 0U));
#pragma endregion
	
#pragma region 멀티 텍스처
	// 왼손
	Load_Texture(L"./Resource/Texture/Player/LeftHand/LeftHand (%d).png", TEX_NORMAL, L"Player", L"Left_Hand", _range<_uint>(0U, 1U));
	// 오른손
	Load_Texture(L"./Resource/Texture/Player/RightHand/RightHand (%d).png", TEX_NORMAL, L"Player", L"Right_Hand", _range<_uint>(0U, 1U));
	// 주먹 차징 (오른손)
	Load_Texture(L"./Resource/Texture/Player/RightHand_Charging/RightHand_Charging (%d).png", TEX_NORMAL, L"Player", L"RightHand_Charging", _range<_uint>(0U, 2U));
	// 버리기 (오른손)
	Load_Texture(L"./Resource/Texture/Player/ThrowHand/UnderThrowRightHand (%d).png", TEX_NORMAL, L"Player", L"UnderThrow_RightHand", _range<_uint>(0U, 0U));
	// 쇠파이프
	Load_Texture(L"./Resource/Texture/Player/Steel_Pipe/Steel_Pipe (%d).png", TEX_NORMAL, L"Player", L"Steel_Pipe", _range<_uint>(0U, 3U));
	// 쇠파이프 차징
	Load_Texture(L"./Resource/Texture/Player/Steel_Pipe_Charging/Steel_Pipe_Charging (%d).png", TEX_NORMAL, L"Player", L"Steel_Pipe_Charging", _range<_uint>(0U, 3U));
	// 프라이팬
	Load_Texture(L"./Resource/Texture/Player/FryingPan/FryingPan (%d).png", TEX_NORMAL, L"Player", L"FryingPan", _range<_uint>(0U, 3U));
	// 프라이팬 차징
	Load_Texture(L"./Resource/Texture/Player/FryingPan_Charging/FryingPan_Charging (%d).png", TEX_NORMAL, L"Player", L"FryingPan_Charging", _range<_uint>(0U, 3U));
	// 라이터
	Load_Texture(L"./Resource/Texture/Player/Zippo/Zippo (%d).png", TEX_NORMAL, L"Player", L"Righter", _range<_uint>(0U, 5U));
	// 플레이어 총
	Load_Texture(L"./Resource/Texture/Player/Gun/Gun (%d).png", TEX_NORMAL, L"Player", L"Gun", _range<_uint>(0U, 3U));
	// 플레이어 총 회전
	Load_Texture(L"./Resource/Texture/Player/GunSpin/Spin (%d).png", TEX_NORMAL, L"Player", L"Gun_Spin", _range<_uint>(0U, 3U));
	// 플레이어 톰슨 기관총
	Load_Texture(L"./Resource/Texture/Player/Thompson/Thompson (%d).png", TEX_NORMAL, L"Player", L"Thompson", _range<_uint>(0U, 3U));
	// 맥주병
	Load_Texture(L"./Resource/Texture/Player/BeerBottle/BeerBottle (%d).png", TEX_NORMAL, L"Player", L"BeerBottle", _range<_uint>(0U, 4U));
	// 발차기
	Load_Texture(L"./Resource/Texture/Player/PlayerKick/PlayerKick (%d).png", TEX_NORMAL, L"Player", L"Kick", _range<_uint>(0U, 2U));
#pragma endregion

#pragma region Brown Monster
	// Monster Texture - Brown - Single : 3EA
	Load_Texture(L"./Resource/Texture/Monster/Brown/Single/CrotchHit.png", TEX_NORMAL, L"Brown_Single", L"CrotchHit");
	Load_Texture(L"./Resource/Texture/Monster/Brown/Stand/StandSouth.png", TEX_NORMAL, L"Brown_Single", L"Stand_South");
	Load_Texture(L"./Resource/Texture/Monster/Brown/Single/Suspicious.png", TEX_NORMAL, L"Brown_Single", L"Suspicious");
	
	// Monster Texture - Brown - Multi : 18EA
	Load_Texture(L"./Resource/Texture/Monster/Brown/BasicAttack/BasicAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"NormalAttack", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"HeavyAttack", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/InchForward/InchForward_%d.png", TEX_NORMAL, L"Brown_Multi", L"InchForward", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Chopped/KarateChopped_%d.png", TEX_NORMAL, L"Brown_Multi", L"Chopped", _range<_uint>(0U, 17U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Brown_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Run/South/RunSouth_%d.png", TEX_NORMAL, L"Brown_Multi", L"RunSouth", _range<_uint>(0U, 20U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Headless/Headless_%d.png", TEX_NORMAL, L"Brown_Multi", L"Headless", _range<_uint>(0U, 15U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Strafing/Strafing_%d.png", TEX_NORMAL, L"Brown_Multi", L"Strafing", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Falling/Falling_%d.png", TEX_NORMAL, L"Brown_Multi", L"Falling", _range<_uint>(0U, 17U)); // FALLING + GETUP 
	Load_Texture(L"./Resource/Texture/Monster/Brown/Rest/IdleReady_%d.png", TEX_NORMAL, L"Brown_Multi", L"Rest", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Dazed/Dazed_%d.png", TEX_NORMAL, L"Brown_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Death/Death_%d.png", TEX_NORMAL, L"Brown_Multi", L"Death", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Getup/Getup_%d.png", TEX_NORMAL, L"Brown_Multi", L"GetUp", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Taunt/Taunt_%d.png", TEX_NORMAL, L"Brown_Multi", L"Taunt", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Hit/Hit_%d.png", TEX_NORMAL, L"Brown_Multi", L"Hit", _range<_uint>(0U, 5U));

	Load_Texture(L"./Resource/Texture/Monster/Brown/Walk/North/WalkNorth_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_North", _range<_uint>(0U, 31U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Walk/NorthWest/NorthWestWalk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_NorthWest", _range<_uint>(0U, 15U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Walk/South/Walk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_South", _range<_uint>(0U, 23U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Walk/SouthWest/SouthWest_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_SouthWest", _range<_uint>(0U, 23U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Walk/West/West_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_West", _range<_uint>(0U, 23U));
#pragma endregion

#pragma region Gray Monster
	// Monster Texture - Gray - Single : 5EA (2 Pending Approval) 
	Load_Texture(L"./Resource/Texture/Monster/Gray/Single/CrotchHit.png", TEX_NORMAL, L"Gray_Single", L"CrotchHit");
	Load_Texture(L"./Resource/Texture/Monster/Gray/Single/Idle.png", TEX_NORMAL, L"Gray_Single", L"Idle"); // SOUTH 
	Load_Texture(L"./Resource/Texture/Monster/Gray/Single/Suspicious.png", TEX_NORMAL, L"Gray_Single", L"Suspicious"); // SOUTH 


	// Monster Texture - Gray - Multi : 21EA
	Load_Texture(L"./Resource/Texture/Monster/Gray/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Gray_Multi", L"HeavyAttack", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/UpRightRun/UpRightRun_%d.png", TEX_NORMAL, L"Gray_Multi", L"UpRightRun", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/WalkNorth/WalkNorth_%d.png", TEX_NORMAL, L"Gray_Multi", L"WalkNorth", _range<_uint>(0U, 22U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Gray_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Frighten/Frighten_%d.png", TEX_NORMAL, L"Gray_Multi", L"Frighten", _range<_uint>(0U, 18U)); // 없는것 
	Load_Texture(L"./Resource/Texture/Monster/Gray/Headless/Headless_%d.png", TEX_NORMAL, L"Gray_Multi", L"Headless", _range<_uint>(0U, 21U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/HeadShot/HeadShot_%d.png", TEX_NORMAL, L"Gray_Multi", L"HeadShot", _range<_uint>(0U, 19U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/SideWalk/SideWalk_%d.png", TEX_NORMAL, L"Gray_Multi", L"SideWalk", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Chopped/Chopped_%d.png", TEX_NORMAL, L"Gray_Multi", L"Chopped", _range<_uint>(0U, 17U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Falling/Falling_%d.png", TEX_NORMAL, L"Gray_Multi", L"Falling", _range<_uint>(0U, 10U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/KeepEye/KeepEye_%d.png", TEX_NORMAL, L"Gray_Multi", L"KeepEye", _range<_uint>(0U, 26U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/YouDie/YouDie_%d.png", TEX_NORMAL, L"Gray_Multi", L"YouDie", _range<_uint>(0U, 11U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Attack/Attack_%d.png", TEX_NORMAL, L"Gray_Multi", L"Attack", _range<_uint>(0U, 8U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Coward/Coward_%d.png", TEX_NORMAL, L"Gray_Multi", L"Coward", _range<_uint>(0U, 6U)); // Hit임 
	Load_Texture(L"./Resource/Texture/Monster/Gray/WalkSouth/Walk_%d.png", TEX_NORMAL, L"Gray_Multi", L"Walk", _range<_uint>(0U, 18U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Taunt/Taunt_%d.png", TEX_NORMAL, L"Gray_Multi", L"Taunt", _range<_uint>(0U, 10U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Dazed/Dazed_%d.png", TEX_NORMAL, L"Gray_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Throw/Throw_%d.png", TEX_NORMAL, L"Gray_Multi", L"Throw", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Block/Block_%d.png", TEX_NORMAL, L"Gray_Multi", L"Block", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Death/Death_%d.png", TEX_NORMAL, L"Gray_Multi", L"Death", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Rest/Rest_%d.png", TEX_NORMAL, L"Gray_Multi", L"Rest", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Run/Run_%d.png", TEX_NORMAL, L"Gray_Multi", L"Run", _range<_uint>(0U, 21U));



	//추가 IDLE 은 IDLE 폴더가 따로있음 

#pragma endregion

#pragma region Boss Monster
	// Monster Texture - Boss - Single
	Load_Texture(L"./Resource/Texture/Monster/Boss/Stand/Idle_NorthWest.png", TEX_NORMAL, L"Boss_Single", L"Idle_NorthWest");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Stand/Idle_SouthWest.png", TEX_NORMAL, L"Boss_Single", L"Idle_SouthWest");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Stand/Idle_North.png", TEX_NORMAL, L"Boss_Single", L"Idle_North");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Stand/Idle_South.png", TEX_NORMAL, L"Boss_Single", L"Idle_South");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Stand/Idle_West.png", TEX_NORMAL, L"Boss_Single", L"Idle_West");
	
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Pre_Attack.png", TEX_NORMAL, L"Boss_Single", L"Pre_Attack");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/SideReady.png", TEX_NORMAL, L"Boss_Single", L"SideReady");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/CrotchHit.png", TEX_NORMAL, L"Boss_Single", L"CrotchHit");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/BackIdle.png", TEX_NORMAL, L"Boss_Single", L"BackIdle");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Hit_A.png", TEX_NORMAL, L"Boss_Single", L"Hit_A");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Hit_B.png", TEX_NORMAL, L"Boss_Single", L"Hit_B");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Chase.png", TEX_NORMAL, L"Boss_Single", L"Chase");
	
	// Monster Texture - Boss - Multi
	Load_Texture(L"./Resource/Texture/Monster/Boss/NorthWestWalk/NorthWestWalk_%d.png", TEX_NORMAL, L"Boss_Multi", L"NorthWestWalk", _range<_uint>(0U, 19U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/CloseAttack/CloseAttack_%d.png", TEX_NORMAL, L"Boss_Multi", L"CloseAttack", _range<_uint>(0U, 5U));
	//Load_Texture(L"./Resource/Texture/Monster/Boss/NorthWalk/NorthWalk_%d.png", TEX_NORMAL, L"Boss_Multi", L"NorthWalk", _range<_uint>(0U, 26U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Reloading/Reloading_%d.png", TEX_NORMAL, L"Boss_Multi", L"Reloading", _range<_uint>(0U, 17U)); 
	Load_Texture(L"./Resource/Texture/Monster/Boss/IdleReady/IdleReady_%d.png", TEX_NORMAL, L"Boss_Multi", L"IdleReady", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Boss_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Shooting/Shooting_%d.png", TEX_NORMAL, L"Boss_Multi", L"Shooting", _range<_uint>(0U, 8U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Falling/Falling_%d.png", TEX_NORMAL, L"Boss_Multi", L"Falling", _range<_uint>(0U, 10U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Evasion/Evasion_%d.png", TEX_NORMAL, L"Boss_Multi", L"Evasion", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Dazed/Dazed_%d.png", TEX_NORMAL, L"Boss_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Button/SetOn_%d.png", TEX_NORMAL, L"Boss_Multi", L"SetOn", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Death/Death_%d.png", TEX_NORMAL, L"Boss_Multi", L"Death", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Shout/Shout_%d.png", TEX_NORMAL, L"Boss_Multi", L"Shout", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Running/Run_%d.png", TEX_NORMAL, L"Boss_Multi", L"Run", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Roll/Roll_%d.png", TEX_NORMAL, L"Boss_Multi", L"Roll", _range<_uint>(0U, 19U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Walk/Walk_%d.png", TEX_NORMAL, L"Boss_Multi", L"Walk", _range<_uint>(0U, 28U));

#pragma endregion

#pragma region Projectile
	// 디버그용
	Load_Texture(L"./Resource/Texture/Object/PoolBalls_0.png", TEX_NORMAL, L"Debug", L"PoolBall"); // 몬스터 공격 디버그용

	Load_Texture(L"./Resource/Texture/Monster/Projectile/ThrowPipe_%d.png", TEX_NORMAL, L"Projectile", L"ThrowPipe", _range<_uint>(0U, 11U)); // Bomb
	Load_Texture(L"./Resource/Texture/idk/Circle.png", TEX_NORMAL, L"Debug", L"Sphere"); // Bomb

#pragma endregion

#pragma region Object 
	// Object - Food
	Load_Texture(L"./Resource/Texture/Item/Food/BananaPeel.png", TEX_NORMAL, L"Food", L"BananaPeel");
	Load_Texture(L"./Resource/Texture/Item/Food/EatenApple.png", TEX_NORMAL, L"Food", L"EatenApple");
	Load_Texture(L"./Resource/Texture/Item/Food/Medkit.png", TEX_NORMAL, L"Food", L"Medkit");
	Load_Texture(L"./Resource/Texture/Item/Food/Banana.png", TEX_NORMAL, L"Food", L"Banana");
	Load_Texture(L"./Resource/Texture/Item/Food/Apple.png", TEX_NORMAL, L"Food", L"Apple");
	Load_Texture(L"./Resource/Texture/Item/Food/Cola.png", TEX_NORMAL, L"Food", L"Cola");

	// Object - Weapon
	Load_Texture(L"./Resource/Texture/Item/Weapon/BrokenFryingPan.png", TEX_NORMAL, L"Weapon", L"BrokenFryingPan");
	Load_Texture(L"./Resource/Texture/Item/Weapon/BrokenTommyGun.png", TEX_NORMAL, L"Weapon", L"BrokenTommyGun");
	Load_Texture(L"./Resource/Texture/Item/Weapon/CrackedBottle.png", TEX_NORMAL, L"Weapon", L"CrackedBottle");
	Load_Texture(L"./Resource/Texture/Item/Weapon/BrokenBottle.png", TEX_NORMAL, L"Weapon", L"BrokenBottle");
	Load_Texture(L"./Resource/Texture/Item/Weapon/BrokenPistol.png", TEX_NORMAL, L"Weapon", L"BrokenPistol");
	Load_Texture(L"./Resource/Texture/Item/Weapon/GasCanister.png", TEX_NORMAL, L"Weapon", L"GasCanister");
	Load_Texture(L"./Resource/Texture/Item/Weapon/BrokenPipe.png", TEX_NORMAL, L"Weapon", L"BrokenPipe");
	Load_Texture(L"./Resource/Texture/Item/Weapon/FryingPan.png", TEX_NORMAL, L"Weapon", L"FryingPan");
	Load_Texture(L"./Resource/Texture/Item/Weapon/TommyGun.png", TEX_NORMAL, L"Weapon", L"TommyGun");
	Load_Texture(L"./Resource/Texture/Item/Weapon/LeadPipe.png", TEX_NORMAL, L"Weapon", L"LeadPipe");
	Load_Texture(L"./Resource/Texture/Item/Weapon/WallPipe.png", TEX_NORMAL, L"Weapon", L"WallPipe");
	Load_Texture(L"./Resource/Texture/Item/Weapon/Pistol.png", TEX_NORMAL, L"Weapon", L"Pistol");
	Load_Texture(L"./Resource/Texture/Item/Weapon/Bottle.png", TEX_NORMAL, L"Weapon", L"Bottle");
	Load_Texture(L"./Resource/Texture/Item/Weapon/Ammo.png", TEX_NORMAL, L"Weapon", L"Ammo");

	//Object - Throw 
	Load_Texture(L"./Resource/Texture/Item/Throw/ScrunchedPaper_0.png", TEX_NORMAL, L"Throw", L"ScrunchedPaper_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/ScrunchedPaper_1.png", TEX_NORMAL, L"Throw", L"ScrunchedPaper_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/PaperBox_0.png", TEX_NORMAL, L"Throw", L"PaperBox_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/PaperBox_1.png", TEX_NORMAL, L"Throw", L"PaperBox_1");

	Load_Texture(L"./Resource/Texture/Item/Throw/Sack_0.png", TEX_NORMAL, L"Throw", L"Sack_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/Sack_1.png", TEX_NORMAL, L"Throw", L"Sack_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/Sack_2.png", TEX_NORMAL, L"Throw", L"Sack_2");
	Load_Texture(L"./Resource/Texture/Item/Throw/Handbag.png", TEX_NORMAL, L"Throw", L"Handbag");
	Load_Texture(L"./Resource/Texture/Item/Throw/Fedora.png", TEX_NORMAL, L"Throw", L"Fedora");
	Load_Texture(L"./Resource/Texture/Item/Throw/Rope.png", TEX_NORMAL, L"Throw", L"Rope");
		
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_0.png", TEX_NORMAL, L"Throw", L"Chair_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_1.png", TEX_NORMAL, L"Throw", L"Chair_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_2.png", TEX_NORMAL, L"Throw", L"Chair_2");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_3.png", TEX_NORMAL, L"Throw", L"Chair_3");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_4.png", TEX_NORMAL, L"Throw", L"Chair_4");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_5.png", TEX_NORMAL, L"Throw", L"Chair_5");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_6.png", TEX_NORMAL, L"Throw", L"Chair_6");
	Load_Texture(L"./Resource/Texture/Item/Throw/Chair_7.png", TEX_NORMAL, L"Throw", L"Chair_7");
	Load_Texture(L"./Resource/Texture/Item/Throw/Board_0.png", TEX_NORMAL, L"Throw", L"Board_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/Board_1.png", TEX_NORMAL, L"Throw", L"Board_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/Board_2.png", TEX_NORMAL, L"Throw", L"Board_2");
	Load_Texture(L"./Resource/Texture/Item/Throw/UmbrellaStand.png", TEX_NORMAL, L"Throw", L"UmbrellaStand");
	Load_Texture(L"./Resource/Texture/Item/Throw/WoodenBarrel.png", TEX_NORMAL, L"Throw", L"WoodenBarrel");
	Load_Texture(L"./Resource/Texture/Item/Throw/WoodenStool.png", TEX_NORMAL, L"Throw", L"WoodenStool");
	Load_Texture(L"./Resource/Texture/Item/Throw/CoatHanger.png", TEX_NORMAL, L"Throw", L"CoatHanger");
	Load_Texture(L"./Resource/Texture/Item/Throw/Mannequin.png", TEX_NORMAL, L"Throw", L"Mannequin");
	Load_Texture(L"./Resource/Texture/Item/Throw/CableReel.png", TEX_NORMAL, L"Throw", L"CableReel");
	Load_Texture(L"./Resource/Texture/Item/Throw/CafeTable.png", TEX_NORMAL, L"Throw", L"CafeTable");
	Load_Texture(L"./Resource/Texture/Item/Throw/CafeSign.png", TEX_NORMAL, L"Throw", L"CafeSign");
	Load_Texture(L"./Resource/Texture/Item/Throw/BarStool.png", TEX_NORMAL, L"Throw", L"BarStool");
	Load_Texture(L"./Resource/Texture/Item/Throw/Toolbox.png", TEX_NORMAL, L"Throw", L"Toolbox");
	Load_Texture(L"./Resource/Texture/Item/Throw/Matches.png", TEX_NORMAL, L"Throw", L"Matches");
		
	Load_Texture(L"./Resource/Texture/Item/Throw/GenericBottles_0.png", TEX_NORMAL, L"Throw", L"GenericBottles_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/GenericBottles_1.png", TEX_NORMAL, L"Throw", L"GenericBottles_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/GenericBottles_2.png", TEX_NORMAL, L"Throw", L"GenericBottles_2");
	Load_Texture(L"./Resource/Texture/Item/Throw/GenericBottles_3.png", TEX_NORMAL, L"Throw", L"GenericBottles_3");
	Load_Texture(L"./Resource/Texture/Item/Throw/GenericBottles_4.png", TEX_NORMAL, L"Throw", L"GenericBottles_4");
	Load_Texture(L"./Resource/Texture/Item/Throw/GenericBottles_5.png", TEX_NORMAL, L"Throw", L"GenericBottles_5");
	Load_Texture(L"./Resource/Texture/Item/Throw/Perfume_0.png", TEX_NORMAL, L"Throw", L"Perfume_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/Perfume_1.png", TEX_NORMAL, L"Throw", L"Perfume_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/Perfume_2.png", TEX_NORMAL, L"Throw", L"Perfume_2");
	Load_Texture(L"./Resource/Texture/Item/Throw/TablePlantFlower.png", TEX_NORMAL, L"Throw", L"TablePlantFlower");
	Load_Texture(L"./Resource/Texture/Item/Throw/TablePlantLeaf.png", TEX_NORMAL, L"Throw", L"TablePlantLeaf");
	Load_Texture(L"./Resource/Texture/Item/Throw/WhiskeyGlass.png", TEX_NORMAL, L"Throw", L"WhiskeyGlass");
	Load_Texture(L"./Resource/Texture/Item/Throw/MartiniGlass.png", TEX_NORMAL, L"Throw", L"MartiniGlass");
	Load_Texture(L"./Resource/Texture/Item/Throw/BeerGlass.png", TEX_NORMAL, L"Throw", L"BeerGlass");
	Load_Texture(L"./Resource/Texture/Item/Throw/Potplant.png", TEX_NORMAL, L"Throw", L"Potplant");
	Load_Texture(L"./Resource/Texture/Item/Throw/BeerMug.png", TEX_NORMAL, L"Throw", L"BeerMug");
	Load_Texture(L"./Resource/Texture/Item/Throw/Lamp_0.png", TEX_NORMAL, L"Throw", L"Lamp_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/Lamp_1.png", TEX_NORMAL, L"Throw", L"Lamp_1");
		
	Load_Texture(L"./Resource/Texture/Item/Throw/Trashbin_0.png", TEX_NORMAL, L"Throw", L"Trashbin_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/Trashbin_1.png", TEX_NORMAL, L"Throw", L"Trashbin_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/Trashbin_2.png", TEX_NORMAL, L"Throw", L"Trashbin_2");

	Load_Texture(L"./Resource/Texture/Item/Throw/TrashCan_0.png", TEX_NORMAL, L"Throw", L"TrashCan_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/TrashCan_1.png", TEX_NORMAL, L"Throw", L"TrashCan_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/TrashCan_2.png", TEX_NORMAL, L"Throw", L"TrashCan_2");

	Load_Texture(L"./Resource/Texture/Item/Throw/PaintCan_0.png", TEX_NORMAL, L"Throw", L"PaintCan_0");
	Load_Texture(L"./Resource/Texture/Item/Throw/PaintCan_1.png", TEX_NORMAL, L"Throw", L"PaintCan_1");
	Load_Texture(L"./Resource/Texture/Item/Throw/PaintCan_2.png", TEX_NORMAL, L"Throw", L"PaintCan_2");
	Load_Texture(L"./Resource/Texture/Item/Throw/PaintCan_3.png", TEX_NORMAL, L"Throw", L"PaintCan_3");

	Load_Texture(L"./Resource/Texture/Item/Throw/ChampagneBucket.png", TEX_NORMAL, L"Throw", L"ChampagneBucket");
	Load_Texture(L"./Resource/Texture/Item/Throw/TrafficBollard.png", TEX_NORMAL, L"Throw", L"TrafficBollard");
	Load_Texture(L"./Resource/Texture/Item/Throw/TrafficCone.png", TEX_NORMAL, L"Throw", L"TrafficCone");
	Load_Texture(L"./Resource/Texture/Item/Throw/MetalBarrel.png", TEX_NORMAL, L"Throw", L"MetalBarrel");
	Load_Texture(L"./Resource/Texture/Item/Throw/Microphone.png", TEX_NORMAL, L"Throw", L"Microphone");
	Load_Texture(L"./Resource/Texture/Item/Throw/OilBarrel.png", TEX_NORMAL, L"Throw", L"OilBarrel");
	Load_Texture(L"./Resource/Texture/Item/Throw/Bucket.png", TEX_NORMAL, L"Throw", L"Bucket");

	Load_Texture(L"./Resource/Texture/Item/Interaction/LightSwitch_Off.png", TEX_NORMAL, L"Interaction", L"LightSwitch_Off");
	Load_Texture(L"./Resource/Texture/Item/Interaction/BlockSwitch_Off.png", TEX_NORMAL, L"Interaction", L"BlockSwitch_Off");
	Load_Texture(L"./Resource/Texture/Item/Interaction/LightSwitch_On.png", TEX_NORMAL, L"Interaction", L"LightSwitch_On");
	Load_Texture(L"./Resource/Texture/Item/Interaction/BlockSwitch_On.png", TEX_NORMAL, L"Interaction", L"BlockSwitch_On");
	Load_Texture(L"./Resource/Texture/Item/Interaction/OneButton_Off.png", TEX_NORMAL, L"Interaction", L"OneButton_Off");
	Load_Texture(L"./Resource/Texture/Item/Interaction/TwoButton_Off.png", TEX_NORMAL, L"Interaction", L"TwoButton_Off");
	Load_Texture(L"./Resource/Texture/Item/Interaction/TwoButton_On.png", TEX_NORMAL, L"Interaction",  L"TwoButton_On");
	Load_Texture(L"./Resource/Texture/Item/Interaction/OneButton_On.png", TEX_NORMAL, L"Interaction", L"OneButton_On");
	Load_Texture(L"./Resource/Texture/Item/Interaction/Padlock_Off.png", TEX_NORMAL, L"Interaction", L"Padlock_Off");
	Load_Texture(L"./Resource/Texture/Item/Interaction/Key_Blue.png", TEX_NORMAL, L"Interaction", L"Building_Key");
	Load_Texture(L"./Resource/Texture/Item/Interaction/Key_Red.png", TEX_NORMAL, L"Interaction", L"Bossroom_Key");
	Load_Texture(L"./Resource/Texture/Item/Interaction/Newspaper.png", TEX_NORMAL, L"Interaction", L"Newspaper");
	Load_Texture(L"./Resource/Texture/Item/Interaction/ComicBook.png", TEX_NORMAL, L"Interaction", L"ComicBook");

#pragma endregion

#pragma region Object - Inventory : 직교투영 해야함 

	Load_Texture(L"./Resource/Texture/Item/Inventory/FryingPan.png", TEX_NORMAL, L"Inventory", L"FryingPan");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Revolver.png", TEX_NORMAL, L"Inventory", L"Revolver");
	Load_Texture(L"./Resource/Texture/Item/Inventory/TommyGun.png", TEX_NORMAL, L"Inventory", L"TommyGun");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Bottle.png", TEX_NORMAL, L"Inventory", L"Bottle");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Banana.png", TEX_NORMAL, L"Inventory", L"Banana");
	Load_Texture(L"./Resource/Texture/Item/Inventory/MedKit.png", TEX_NORMAL, L"Inventory", L"MedKit");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Apple.png", TEX_NORMAL, L"Inventory", L"Apple");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Pipe.png", TEX_NORMAL, L"Inventory", L"Pipe");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Cola.png", TEX_NORMAL, L"Inventory", L"Cola");

#pragma endregion

#pragma region Effect
	
	//몬스터 관련 이펙트 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/Stone/FallingStone1.png", TEX_NORMAL, L"Effect", L"Stone1");
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/Stone/FallingStone2.png", TEX_NORMAL, L"Effect", L"Stone2");						
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/SlowThunder/SlowThunder_%d.png", TEX_NORMAL, L"Effect", L"SlowThunder", _range<_uint>(0U, 5U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/MissTarget/MissTarget_%d.png", TEX_NORMAL, L"Effect", L"MissTarget", _range<_uint>(0U, 14U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/EnergyBall/EnergyBall_%d.png", TEX_NORMAL, L"Effect", L"EnergyBall", _range<_uint>(0U, 7U));  
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/RedThunder/RedThunder_%d.png", TEX_NORMAL, L"Effect", L"RedThunder", _range<_uint>(0U, 5U));  
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/FireEffect/Fire_%d.png", TEX_NORMAL, L"Effect", L"FireEffect", _range<_uint>(0U, 5U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/Stone/Stone_%d.png", TEX_NORMAL, L"Effect", L"FallingStone", _range<_uint>(0U, 3U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/Barrier/Barrier_%d.png", TEX_NORMAL, L"Effect", L"Barrier", _range<_uint>(0U, 7U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/GreenBuff/Buff_%d.png", TEX_NORMAL, L"Effect", L"Buff", _range<_uint>(0U, 7U));  
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/Razer/Razer_%d.png", TEX_NORMAL, L"Effect", L"Razer", _range<_uint>(0U, 18U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/SkillEffect/Holy/Holy_%d.png", TEX_NORMAL, L"Effect", L"Holy", _range<_uint>(0U, 3U)); 
	
	Load_Texture(L"./Resource/Texture/AceEffect/Awareness/Awareness_15.png", TEX_NORMAL, L"Effect", L"MaxAware"); // 몬스터 공격 디버그용
	Load_Texture(L"./Resource/Texture/AceEffect/Awareness/Awareness_%d.png", TEX_NORMAL, L"Effect", L"Awareness", _range<_uint>(0U, 15U));

	// 히트판정
	Load_Texture(L"./Resource/Texture/AceEffect/FallingDust/FallingDust_%d.png", TEX_NORMAL, L"Effect", L"FallingDust", _range<_uint>(0U, 17U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/HitBlood/HitBlood_%d.png", TEX_NORMAL, L"Effect", L"HitBlood", _range<_uint>(0U, 13U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/Dazed/DazeSwirl_%d.png", TEX_NORMAL, L"Effect", L"DazeSwirl", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/AceEffect/HitDust/HitDust_%d.png", TEX_NORMAL, L"Effect", L"HitDust", _range<_uint>(0U, 16U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/HitPow/HitPow_%d.png", TEX_NORMAL, L"Effect", L"HitPow", _range<_uint>(0U, 3U)); 
	
	// 폭발
	Load_Texture(L"./Resource/Texture/AceEffect/Explosion/Explosion (%d).png", TEX_NORMAL, L"Effect", L"Explosion", _range<_uint>(0U, 71U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/Lightning/Lightning (%d).png", TEX_NORMAL, L"Effect", L"Lightning", _range<_uint>(0U, 3U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/Bubble/Bubble_%d.png", TEX_NORMAL, L"Effect", L"Bubble", _range<_uint>(0U, 7U)); 

	// 플레이어 총이펙트
	Load_Texture(L"./Resource/Texture/AceEffect/CartridgeCase/Cartridge_%d.png", TEX_NORMAL, L"Effect", L"Cartridge", _range<_uint>(0U, 9U)); 
	Load_Texture(L"./Resource/Texture/AceEffect/Hole/BulletHoles1.png", TEX_NORMAL, L"Effect", L"BulletHoles1"); 
	Load_Texture(L"./Resource/Texture/AceEffect/Hole/BulletHoles2.png", TEX_NORMAL, L"Effect", L"BulletHoles2"); 


#pragma endregion 

#pragma region Building - TEX_CUBE 
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/A.dds", TEX_CUBE, L"Building", L"A");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/B.dds", TEX_CUBE, L"Building", L"B");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/C.dds", TEX_CUBE, L"Building", L"C");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/D.dds", TEX_CUBE, L"Building", L"D");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/E.dds", TEX_CUBE, L"Building", L"E");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/F.dds", TEX_CUBE, L"Building", L"F");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/G.dds", TEX_CUBE, L"Building", L"G");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/H.dds", TEX_CUBE, L"Building", L"H");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/I.dds", TEX_CUBE, L"Building", L"I");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/J.dds", TEX_CUBE, L"Building", L"J");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/K.dds", TEX_CUBE, L"Building", L"K");
	Load_Texture(L"./Resource/Texture/CubeTexture/Building/L.dds", TEX_CUBE, L"Building", L"L");

#pragma endregion

#pragma region Furniture - TEX_CUBE 
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/WhiteCurtainTable.dds", TEX_CUBE, L"Furniture", L"WhiteCurtainTable");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/CigaretteMachine.dds", TEX_CUBE, L"Furniture", L"CigaretteMachine");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/DarkNavyCushion2.dds", TEX_CUBE, L"Furniture", L"DarkNavyCushion2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BrokenVent_Rect.dds", TEX_CUBE, L"Furniture", L"BrokenVent_Rect");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/DarkNavyCushion.dds", TEX_CUBE, L"Furniture", L"DarkNavyCushion");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/DarkRedCushion.dds", TEX_CUBE, L"Furniture", L"DarkRedCushion");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BrokenVentFan.dds", TEX_CUBE, L"Furniture", L"BrokenVentFan");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/GreenCabinet2.dds", TEX_CUBE, L"Furniture", L"GreenCabinet2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/WhiteStorage2.dds", TEX_CUBE, L"Furniture", L"WhiteStorage2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/electricalBox.dds", TEX_CUBE, L"Furniture", L"electricalBox");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/GreenCabinet.dds", TEX_CUBE, L"Furniture", L"GreenCabinet");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/MarbleTable2.dds", TEX_CUBE, L"Furniture", L"MarbleTable2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/WhiteStorage.dds", TEX_CUBE, L"Furniture", L"WhiteStorage");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/MarbleTable1.dds", TEX_CUBE, L"Furniture", L"MarbleTable1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BookShelves2.dds", TEX_CUBE, L"Furniture", L"BookShelves2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/CockMachine.dds", TEX_CUBE, L"Furniture", L"CockMachine");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/GrayCabinet.dds", TEX_CUBE, L"Furniture", L"GrayCabinet");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/GreenCrate1.dds", TEX_CUBE, L"Furniture", L"GreenCrate1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/WoodCabinet.dds", TEX_CUBE, L"Furniture", L"WoodCabinet");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/RedCushion2.dds", TEX_CUBE, L"Furniture", L"RedCushion2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BlindWindow.dds", TEX_CUBE, L"Furniture", L"BlindWindow");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BlueCabinet.dds", TEX_CUBE, L"Furniture", L"BlueCabinet");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BlueMachine.dds", TEX_CUBE, L"Furniture", L"BlueMachine");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BookShelves.dds", TEX_CUBE, L"Furniture", L"BookShelves");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BrokenGlass.dds", TEX_CUBE, L"Furniture", L"BrokenGlass");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/PhoneBooth.dds", TEX_CUBE, L"Furniture", L"PhoneBooth");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BrownTrunk.dds", TEX_CUBE, L"Furniture", L"BrownTrunk");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/RedCurtain.dds", TEX_CUBE, L"Furniture", L"RedCurtain");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/RedCushion.dds", TEX_CUBE, L"Furniture", L"RedCushion");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Television.dds", TEX_CUBE, L"Furniture", L"Television");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/WhiteStand.dds", TEX_CUBE, L"Furniture", L"WhiteStand");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/VentIntro.dds", TEX_CUBE, L"Furniture", L"VentIntro");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Barbottle.dds", TEX_CUBE, L"Furniture", L"BarBottle");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BlueTrunk.dds", TEX_CUBE, L"Furniture", L"BlueTrunk");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/TableLeg.dds", TEX_CUBE, L"Furniture", L"TableLeg");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/VentFan1.dds", TEX_CUBE, L"Furniture", L"VentFan1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/VentFan2.dds", TEX_CUBE, L"Furniture", L"VentFan2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/LightBox.dds", TEX_CUBE, L"Furniture", L"LightBox");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Radiator.dds", TEX_CUBE, L"Furniture", L"Radiator");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BarFront.dds", TEX_CUBE, L"Furniture", L"BarFront");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/BarTable.dds", TEX_CUBE, L"Furniture", L"BarTable");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Cabinet1.dds", TEX_CUBE, L"Furniture", L"Cabinet1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Cabinet2.dds", TEX_CUBE, L"Furniture", L"Cabinet2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Cabinet3.dds", TEX_CUBE, L"Furniture", L"Cabinet3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column10.dds", TEX_CUBE, L"Furniture", L"Column10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column1.dds", TEX_CUBE, L"Furniture", L"Column1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column2.dds", TEX_CUBE, L"Furniture", L"Column2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column3.dds", TEX_CUBE, L"Furniture", L"Column3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column4.dds", TEX_CUBE, L"Furniture", L"Column4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column5.dds", TEX_CUBE, L"Furniture", L"Column5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column6.dds", TEX_CUBE, L"Furniture", L"Column6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column7.dds", TEX_CUBE, L"Furniture", L"Column7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column8.dds", TEX_CUBE, L"Furniture", L"Column8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Column9.dds", TEX_CUBE, L"Furniture", L"Column9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Closet.dds", TEX_CUBE, L"Furniture", L"Closet");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Crate1.dds", TEX_CUBE, L"Furniture", L"Crate1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Crate2.dds", TEX_CUBE, L"Furniture", L"Crate2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Crate3.dds", TEX_CUBE, L"Furniture", L"Crate3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Crate4.dds", TEX_CUBE, L"Furniture", L"Crate4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Glass1.dds", TEX_CUBE, L"Furniture", L"Glass1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Glass2.dds", TEX_CUBE, L"Furniture", L"Glass2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Clock.dds", TEX_CUBE, L"Furniture", L"Clock");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Radio.dds", TEX_CUBE, L"Furniture", L"Radio");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Stage.dds", TEX_CUBE, L"Furniture", L"Stage");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Train.dds", TEX_CUBE, L"Furniture", L"Train");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Gate1.dds", TEX_CUBE, L"Furniture", L"Gate1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Furniture/Desk.dds", TEX_CUBE, L"Furniture", L"Desk");


#pragma endregion

#pragma region Structure - TEX_CUBE 
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Aspart1.dds", TEX_CUBE, L"Structure", L"Aspart1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Aspart2.dds", TEX_CUBE, L"Structure", L"Aspart2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Column.dds", TEX_CUBE, L"Structure", L"Column");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_1.dds", TEX_CUBE, L"Structure", L"Door_1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_2.dds", TEX_CUBE, L"Structure", L"Door_2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_3.dds", TEX_CUBE, L"Structure", L"Door_3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_4.dds", TEX_CUBE, L"Structure", L"Door_4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_5.dds", TEX_CUBE, L"Structure", L"Door_5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_6.dds", TEX_CUBE, L"Structure", L"Door_6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_7.dds", TEX_CUBE, L"Structure", L"Door_7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_8.dds", TEX_CUBE, L"Structure", L"Door_8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_9.dds", TEX_CUBE, L"Structure", L"Door_9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_10.dds", TEX_CUBE, L"Structure", L"Door_10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_11.dds", TEX_CUBE, L"Structure", L"Door_11");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_12.dds", TEX_CUBE, L"Structure", L"Door_12");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_13.dds", TEX_CUBE, L"Structure", L"Door_13");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_14.dds", TEX_CUBE, L"Structure", L"Door_14");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_15.dds", TEX_CUBE, L"Structure", L"Door_15");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_16.dds", TEX_CUBE, L"Structure", L"Door_16");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Door_17.dds", TEX_CUBE, L"Structure", L"Door_17");

	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Manhole.dds", TEX_CUBE, L"Structure", L"Manhole");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_1.dds", TEX_CUBE, L"Structure", L"Metal_1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_2.dds", TEX_CUBE, L"Structure", L"Metal_2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_3.dds", TEX_CUBE, L"Structure", L"Metal_3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_4.dds", TEX_CUBE, L"Structure", L"Metal_4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_5.dds", TEX_CUBE, L"Structure", L"Metal_5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_6.dds", TEX_CUBE, L"Structure", L"Metal_6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_7.dds", TEX_CUBE, L"Structure", L"Metal_7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_8.dds", TEX_CUBE, L"Structure", L"Metal_8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_9.dds", TEX_CUBE, L"Structure", L"Metal_9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_10.dds", TEX_CUBE, L"Structure", L"Metal_10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_11.dds", TEX_CUBE, L"Structure", L"Metal_11");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_12.dds", TEX_CUBE, L"Structure", L"Metal_12");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_13.dds", TEX_CUBE, L"Structure", L"Metal_13");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_14.dds", TEX_CUBE, L"Structure", L"Metal_14");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_15.dds", TEX_CUBE, L"Structure", L"Metal_15");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_16.dds", TEX_CUBE, L"Structure", L"Metal_16");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_17.dds", TEX_CUBE, L"Structure", L"Metal_17");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_18.dds", TEX_CUBE, L"Structure", L"Metal_18");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_19.dds", TEX_CUBE, L"Structure", L"Metal_19");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_20.dds", TEX_CUBE, L"Structure", L"Metal_20");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_21.dds", TEX_CUBE, L"Structure", L"Metal_21");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_22.dds", TEX_CUBE, L"Structure", L"Metal_22");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_23.dds", TEX_CUBE, L"Structure", L"Metal_23");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_24.dds", TEX_CUBE, L"Structure", L"Metal_24");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_25.dds", TEX_CUBE, L"Structure", L"Metal_25");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_26.dds", TEX_CUBE, L"Structure", L"Metal_26");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_27.dds", TEX_CUBE, L"Structure", L"Metal_27");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Metal_28.dds", TEX_CUBE, L"Structure", L"Metal_28");

	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Mosiac.dds", TEX_CUBE, L"Structure", L"Mosiac");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Thing.dds", TEX_CUBE, L"Structure", L"Thing");

	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_1.dds", TEX_CUBE, L"Structure", L"TileA_1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_2.dds", TEX_CUBE, L"Structure", L"TileA_2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_3.dds", TEX_CUBE, L"Structure", L"TileA_3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_4.dds", TEX_CUBE, L"Structure", L"TileA_4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_5.dds", TEX_CUBE, L"Structure", L"TileA_5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_6.dds", TEX_CUBE, L"Structure", L"TileA_6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_7.dds", TEX_CUBE, L"Structure", L"TileA_7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_8.dds", TEX_CUBE, L"Structure", L"TileA_8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_9.dds", TEX_CUBE, L"Structure", L"TileA_9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_10.dds", TEX_CUBE, L"Structure", L"TileA_10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_11.dds", TEX_CUBE, L"Structure", L"TileA_11");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_12.dds", TEX_CUBE, L"Structure", L"TileA_12");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_13.dds", TEX_CUBE, L"Structure", L"TileA_13");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_14.dds", TEX_CUBE, L"Structure", L"TileA_14");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_15.dds", TEX_CUBE, L"Structure", L"TileA_15");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_16.dds", TEX_CUBE, L"Structure", L"TileA_16");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_17.dds", TEX_CUBE, L"Structure", L"TileA_17");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_18.dds", TEX_CUBE, L"Structure", L"TileA_18");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_19.dds", TEX_CUBE, L"Structure", L"TileA_19");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_20.dds", TEX_CUBE, L"Structure", L"TileA_20");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_21.dds", TEX_CUBE, L"Structure", L"TileA_21");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_22.dds", TEX_CUBE, L"Structure", L"TileA_22");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_23.dds", TEX_CUBE, L"Structure", L"TileA_23");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_24.dds", TEX_CUBE, L"Structure", L"TileA_24");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileA_25.dds", TEX_CUBE, L"Structure", L"TileA_25");

	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_1.dds", TEX_CUBE, L"Structure", L"TileB_1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_2.dds", TEX_CUBE, L"Structure", L"TileB_2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_3.dds", TEX_CUBE, L"Structure", L"TileB_3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_4.dds", TEX_CUBE, L"Structure", L"TileB_4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_5.dds", TEX_CUBE, L"Structure", L"TileB_5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_6.dds", TEX_CUBE, L"Structure", L"TileB_6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_7.dds", TEX_CUBE, L"Structure", L"TileB_7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_8.dds", TEX_CUBE, L"Structure", L"TileB_8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_9.dds", TEX_CUBE, L"Structure", L"TileB_9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_10.dds", TEX_CUBE, L"Structure", L"TileB_10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_11.dds", TEX_CUBE, L"Structure", L"TileB_11");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_12.dds", TEX_CUBE, L"Structure", L"TileB_12");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_13.dds", TEX_CUBE, L"Structure", L"TileB_13");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_14.dds", TEX_CUBE, L"Structure", L"TileB_14");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_15.dds", TEX_CUBE, L"Structure", L"TileB_15");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_16.dds", TEX_CUBE, L"Structure", L"TileB_16");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_17.dds", TEX_CUBE, L"Structure", L"TileB_17");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_18.dds", TEX_CUBE, L"Structure", L"TileB_18");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_19.dds", TEX_CUBE, L"Structure", L"TileB_19");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_20.dds", TEX_CUBE, L"Structure", L"TileB_20");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_21.dds", TEX_CUBE, L"Structure", L"TileB_21");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_22.dds", TEX_CUBE, L"Structure", L"TileB_22");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_23.dds", TEX_CUBE, L"Structure", L"TileB_23");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_24.dds", TEX_CUBE, L"Structure", L"TileB_24");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_25.dds", TEX_CUBE, L"Structure", L"TileB_25");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_26.dds", TEX_CUBE, L"Structure", L"TileB_26");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_27.dds", TEX_CUBE, L"Structure", L"TileB_27");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/TileB_28.dds", TEX_CUBE, L"Structure", L"TileB_28");

	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_1.dds", TEX_CUBE, L"Structure", L"Wall_1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_2.dds", TEX_CUBE, L"Structure", L"Wall_2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_3.dds", TEX_CUBE, L"Structure", L"Wall_3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_4.dds", TEX_CUBE, L"Structure", L"Wall_4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_5.dds", TEX_CUBE, L"Structure", L"Wall_5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_6.dds", TEX_CUBE, L"Structure", L"Wall_6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_7.dds", TEX_CUBE, L"Structure", L"Wall_7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_8.dds", TEX_CUBE, L"Structure", L"Wall_8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_9.dds", TEX_CUBE, L"Structure", L"Wall_9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_10.dds", TEX_CUBE, L"Structure", L"Wall_10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_11.dds", TEX_CUBE, L"Structure", L"Wall_11");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_12.dds", TEX_CUBE, L"Structure", L"Wall_12");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_13.dds", TEX_CUBE, L"Structure", L"Wall_13");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_14.dds", TEX_CUBE, L"Structure", L"Wall_14");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_15.dds", TEX_CUBE, L"Structure", L"Wall_15");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_16.dds", TEX_CUBE, L"Structure", L"Wall_16");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_17.dds", TEX_CUBE, L"Structure", L"Wall_17");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_18.dds", TEX_CUBE, L"Structure", L"Wall_18");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_19.dds", TEX_CUBE, L"Structure", L"Wall_19");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_20.dds", TEX_CUBE, L"Structure", L"Wall_20");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_21.dds", TEX_CUBE, L"Structure", L"Wall_21");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_22.dds", TEX_CUBE, L"Structure", L"Wall_22");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_23.dds", TEX_CUBE, L"Structure", L"Wall_23");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_24.dds", TEX_CUBE, L"Structure", L"Wall_24");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_25.dds", TEX_CUBE, L"Structure", L"Wall_25");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_26.dds", TEX_CUBE, L"Structure", L"Wall_26");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_27.dds", TEX_CUBE, L"Structure", L"Wall_27");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_28.dds", TEX_CUBE, L"Structure", L"Wall_28");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_29.dds", TEX_CUBE, L"Structure", L"Wall_29");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_30.dds", TEX_CUBE, L"Structure", L"Wall_30");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_31.dds", TEX_CUBE, L"Structure", L"Wall_31");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_32.dds", TEX_CUBE, L"Structure", L"Wall_32");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_33.dds", TEX_CUBE, L"Structure", L"Wall_33");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_34.dds", TEX_CUBE, L"Structure", L"Wall_34");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_35.dds", TEX_CUBE, L"Structure", L"Wall_35");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_36.dds", TEX_CUBE, L"Structure", L"Wall_36");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_37.dds", TEX_CUBE, L"Structure", L"Wall_37");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_38.dds", TEX_CUBE, L"Structure", L"Wall_38");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_39.dds", TEX_CUBE, L"Structure", L"Wall_39");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_40.dds", TEX_CUBE, L"Structure", L"Wall_40");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_41.dds", TEX_CUBE, L"Structure", L"Wall_41");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_42.dds", TEX_CUBE, L"Structure", L"Wall_42");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_43.dds", TEX_CUBE, L"Structure", L"Wall_43");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_44.dds", TEX_CUBE, L"Structure", L"Wall_44");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_45.dds", TEX_CUBE, L"Structure", L"Wall_45");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_46.dds", TEX_CUBE, L"Structure", L"Wall_46");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_47.dds", TEX_CUBE, L"Structure", L"Wall_47");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_48.dds", TEX_CUBE, L"Structure", L"Wall_48");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_49.dds", TEX_CUBE, L"Structure", L"Wall_49");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_50.dds", TEX_CUBE, L"Structure", L"Wall_50");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_51.dds", TEX_CUBE, L"Structure", L"Wall_51");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_52.dds", TEX_CUBE, L"Structure", L"Wall_52");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_53.dds", TEX_CUBE, L"Structure", L"Wall_53");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_54.dds", TEX_CUBE, L"Structure", L"Wall_54");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_55.dds", TEX_CUBE, L"Structure", L"Wall_55");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_56.dds", TEX_CUBE, L"Structure", L"Wall_56");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_57.dds", TEX_CUBE, L"Structure", L"Wall_57");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_58.dds", TEX_CUBE, L"Structure", L"Wall_58");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_59.dds", TEX_CUBE, L"Structure", L"Wall_59");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_60.dds", TEX_CUBE, L"Structure", L"Wall_60");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_61.dds", TEX_CUBE, L"Structure", L"Wall_61");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_62.dds", TEX_CUBE, L"Structure", L"Wall_62");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_63.dds", TEX_CUBE, L"Structure", L"Wall_63");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_64.dds", TEX_CUBE, L"Structure", L"Wall_64");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_65.dds", TEX_CUBE, L"Structure", L"Wall_65");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_66.dds", TEX_CUBE, L"Structure", L"Wall_66");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wall_67.dds", TEX_CUBE, L"Structure", L"Wall_67");

	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_1.dds", TEX_CUBE, L"Structure", L"Wood_1");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_2.dds", TEX_CUBE, L"Structure", L"Wood_2");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_3.dds", TEX_CUBE, L"Structure", L"Wood_3");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_4.dds", TEX_CUBE, L"Structure", L"Wood_4");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_5.dds", TEX_CUBE, L"Structure", L"Wood_5");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_6.dds", TEX_CUBE, L"Structure", L"Wood_6");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_7.dds", TEX_CUBE, L"Structure", L"Wood_7");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_8.dds", TEX_CUBE, L"Structure", L"Wood_8");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_9.dds", TEX_CUBE, L"Structure", L"Wood_9");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_10.dds", TEX_CUBE, L"Structure", L"Wood_10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_11.dds", TEX_CUBE, L"Structure", L"Wood_11");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_12.dds", TEX_CUBE, L"Structure", L"Wood_12");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_13.dds", TEX_CUBE, L"Structure", L"Wood_13");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_14.dds", TEX_CUBE, L"Structure", L"Wood_14");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_15.dds", TEX_CUBE, L"Structure", L"Wood_15");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_16.dds", TEX_CUBE, L"Structure", L"Wood_16");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_17.dds", TEX_CUBE, L"Structure", L"Wood_17");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_18.dds", TEX_CUBE, L"Structure", L"Wood_18");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_19.dds", TEX_CUBE, L"Structure", L"Wood_19");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_20.dds", TEX_CUBE, L"Structure", L"Wood_20");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_21.dds", TEX_CUBE, L"Structure", L"Wood_21");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_22.dds", TEX_CUBE, L"Structure", L"Wood_22");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_23.dds", TEX_CUBE, L"Structure", L"Wood_23");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_24.dds", TEX_CUBE, L"Structure", L"Wood_24");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_25.dds", TEX_CUBE, L"Structure", L"Wood_25");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_26.dds", TEX_CUBE, L"Structure", L"Wood_26");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_27.dds", TEX_CUBE, L"Structure", L"Wood_27");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_28.dds", TEX_CUBE, L"Structure", L"Wood_28");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_29.dds", TEX_CUBE, L"Structure", L"Wood_29");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_30.dds", TEX_CUBE, L"Structure", L"Wood_30");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_31.dds", TEX_CUBE, L"Structure", L"Wood_31");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_32.dds", TEX_CUBE, L"Structure", L"Wood_32");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_33.dds", TEX_CUBE, L"Structure", L"Wood_33");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_34.dds", TEX_CUBE, L"Structure", L"Wood_34");
	Load_Texture(L"./Resource/Texture/CubeTexture/Structure/Wood_35.dds", TEX_CUBE, L"Structure", L"Wood_35");

#pragma endregion

#pragma region fourth  - TEX_CUBE 
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/YellowLight.dds", TEX_CUBE, L"Light", L"YellowLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/WhiteLight.dds", TEX_CUBE, L"Light", L"WhiteLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/BlueLight.dds", TEX_CUBE, L"Light", L"BlueLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/RedLight.dds", TEX_CUBE, L"Light", L"RedLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/light10.dds", TEX_CUBE, L"Light", L"light10");
	
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/BlockSwitch_Off.dds", TEX_CUBE, L"TriggerCube", L"BlockSwitch_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/LightSwitch_Off.dds", TEX_CUBE, L"TriggerCube", L"LightSwitch_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/BlockSwitch_On.dds", TEX_CUBE, L"TriggerCube", L"BlockSwitch_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/LightSwitch_On.dds", TEX_CUBE, L"TriggerCube", L"LightSwitch_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/OneButton_Off.dds", TEX_CUBE, L"TriggerCube", L"OneButton_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/PadlockBroken.dds", TEX_CUBE, L"TriggerCube", L"PadlockBroken");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/TwoButton_Off.dds", TEX_CUBE, L"TriggerCube", L"TwoButton_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/OneButton_On.dds", TEX_CUBE, L"TriggerCube", L"OneButton_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/PadlockChain.dds", TEX_CUBE, L"TriggerCube", L"PadlockChain");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/TwoButton_On.dds", TEX_CUBE, L"TriggerCube", L"TwoButton_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/MothPainting.dds", TEX_CUBE, L"TriggerCube", L"MothPainting");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/Shelves_dark.dds", TEX_CUBE, L"TriggerCube", L"Shelves_dark");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/Padlock_Off.dds", TEX_CUBE, L"TriggerCube", L"Padlock_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/ComicBook.dds", TEX_CUBE, L"TriggerCube", L"ComicBook");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/Newspaper.dds", TEX_CUBE, L"TriggerCube", L"Newspaper");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/Key_Blue.dds", TEX_CUBE, L"TriggerCube", L"Key_Blue");
	Load_Texture(L"./Resource/Texture/CubeTexture/fourth/Key_Red.dds", TEX_CUBE, L"TriggerCube", L"Key_Red");
	
#pragma endregion

#pragma region UI  
	// HudBox
	Load_Texture(L"./Resource/Texture/UI/PlayerHudBox_Highlight.png", TEX_NORMAL, L"UI", L"HudBox_Highlight");
	Load_Texture(L"./Resource/Texture/UI/PlayerHudBox_%d.png", TEX_NORMAL, L"UI", L"PlayerHudBox", _range<_uint>(0U, 2U));

	//CrossHair
	Load_Texture(L"./Resource/Texture/crosshair/CrosshairHitDistanceIndicator.png", TEX_NORMAL, L"UI_Crosshair", L"Hit");
	Load_Texture(L"./Resource/Texture/crosshair/CrosshairHeavyWindup.png", TEX_NORMAL, L"UI_Crosshair", L"Windup");
	Load_Texture(L"./Resource/Texture/crosshair/CrosshairHeavyReady.png", TEX_NORMAL, L"UI_Crosshair", L"Charge");
	Load_Texture(L"./Resource/Texture/crosshair/CrosshairSpinning.png", TEX_NORMAL, L"UI_Crosshair", L"Attack");
	Load_Texture(L"./Resource/Texture/crosshair/CrosshairSide.png", TEX_NORMAL, L"UI_Crosshair", L"Side");
	Load_Texture(L"./Resource/Texture/crosshair/CrosshairDot.png", TEX_NORMAL, L"UI_Crosshair", L"Gun");
	Load_Texture(L"./Resource/Texture/crosshair/Crosshair.png", TEX_NORMAL, L"UI_Crosshair", L"Basic");

	// PlayerFace
	Load_Texture(L"./Resource/Texture/UI/SlightlyHurt_%d.png", TEX_NORMAL, L"UI_Face", L"SlightlyHurt", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/UI/ModeratelyHurt_%d.png", TEX_NORMAL, L"UI_Face", L"ModeratelyHurt", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/UI/BadlyHurt_%d.png", TEX_NORMAL, L"UI_Face", L"BadlyHurt", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/UI/Healthy_%d.png", TEX_NORMAL, L"UI_Face", L"Healthy", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/UI/Hurt_%d.png", TEX_NORMAL, L"UI_Face", L"Hurt", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/UI/DeadFace.png", TEX_NORMAL, L"UI_Face", L"DeadFace");

	// Fade
	Load_Texture(L"./Resource/Texture/Scene/Fade_%d.png", TEX_NORMAL, L"UI_Scene", L"FadeInOut", _range<_uint>(0U, 30U));
	
	// Green Effect - 아이템 획득
	Load_Texture(L"./Resource/Texture/UI/EatEffect_%d.png", TEX_NORMAL, L"UI_Scene", L"EatEffect", _range<_uint>(0U, 7U));
	 
	// Red Effect - 플레이어 피격
	Load_Texture(L"./Resource/Texture/UI/PlayerHurt_%d.png", TEX_NORMAL, L"UI_Scene", L"PlayerHurt", _range<_uint>(0U, 7U));

	// Monster Hp Bar
	Load_Texture(L"./Resource/Texture/UI/HealthBar_Border.png", TEX_NORMAL, L"UI_HpBar", L"Border");
	Load_Texture(L"./Resource/Texture/UI/PreHealth.png", TEX_NORMAL, L"UI_HpBar", L"PreHealth");
	Load_Texture(L"./Resource/Texture/UI/Health.png", TEX_NORMAL, L"UI_HpBar", L"Health");
	Load_Texture(L"./Resource/Texture/UI/Stamina.png", TEX_NORMAL, L"UI_HpBar", L"Stamina");

	Load_Texture(L"./Resource/Texture/UI/Journal.png", TEX_NORMAL, L"UI_Panel", L"Journal");

	// NPC
	Load_Texture(L"./Resource/Texture/NPC/Hegrid.png", TEX_NORMAL, L"NPC", L"Hagrid");

#pragma endregion

#pragma region 4차

	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/BlockSwitch_Off.dds", TEX_NORMAL, L"Fourth", L"BlockSwitch_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/BlockSwitch_On.dds", TEX_NORMAL, L"Fourth", L"BlockSwitch_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/BlueLight.dds", TEX_NORMAL, L"Fourth", L"BlueLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/ComicBook.dds", TEX_NORMAL, L"Fourth", L"ComicBook");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/Key_Blue.dds", TEX_NORMAL, L"Fourth", L"Key_Blue");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/Key_Red.dds", TEX_NORMAL, L"Fourth", L"Key_Red");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/light10.dds", TEX_NORMAL, L"Fourth", L"light10");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/LightSwitch_Off.dds", TEX_NORMAL, L"Fourth", L"LightSwitch_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/LightSwitch_On.dds", TEX_NORMAL, L"Fourth", L"LightSwitch_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/MothPainting.dds", TEX_NORMAL, L"Fourth", L"MothPainting");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/Newspaper.dds", TEX_NORMAL, L"Fourth", L"Newspaper");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/OneButton_Off.dds", TEX_NORMAL, L"Fourth", L"OneButton_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/OneButton_On.dds", TEX_NORMAL, L"Fourth", L"OneButton_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/Padlock_Off.dds", TEX_NORMAL, L"Fourth", L"Padlock_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/PadlockBroken.dds", TEX_NORMAL, L"Fourth", L"PadlockBroken");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/PadlockChain.dds", TEX_NORMAL, L"Fourth", L"PadlockChain");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/RedLight.dds", TEX_NORMAL, L"Fourth", L"RedLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/Shelves_dark.dds", TEX_NORMAL, L"Fourth", L"Shelves_dark");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/TwoButton_Off.dds", TEX_NORMAL, L"Fourth", L"TwoButton_Off");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/TwoButton_On.dds", TEX_NORMAL, L"Fourth", L"TwoButton_On");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/WhiteLight.dds", TEX_NORMAL, L"Fourth", L"WhiteLight");
	Load_Texture(L"./Resource/Texture/CubeTexture/Fourth/YellowLight.dds", TEX_NORMAL, L"Fourth", L"YellowLight");

#pragma endregion


	Wait_LoadTextureAsync();

	return S_OK;
}

void CLoading::Load_TextureAsync(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange)
{
	Engine::Ready_Texture(pfilePath, eID, pGroupName, pTextureName, iCntRange);
}

void CLoading::Load_Texture(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange, _bool bAsync)
{
	if (bAsync)
	{
		// 쓰레드 만큼만 만들도록 하자. 물론 MultiTexture에서도 만들긴 하지만... 좀더 낫겠지
		if (m_listAsyncTexture.size() > m_iFutureLimit)
		{
			for (auto iter = m_listAsyncTexture.begin(); iter != m_listAsyncTexture.end();)
			{
				future_status status = (*iter).wait_for(0.01ms);
				if (status == future_status::ready)
				{
					(*iter).get();
					iter = m_listAsyncTexture.erase(iter);
				}
				else
					++iter;

				if (m_listAsyncTexture.size() != 0LL && iter == m_listAsyncTexture.end())
					iter = m_listAsyncTexture.begin();
			}
			
		}
		m_listAsyncTexture.push_back(async(launch::async, &CLoading::Load_TextureAsync, this, pfilePath, eID, pGroupName, pTextureName, iCntRange));
	}
	else
		Engine::Ready_Texture(pfilePath, eID, pGroupName, pTextureName, iCntRange);
}

void CLoading::Wait_LoadTextureAsync()
{
	for (auto iter = m_listAsyncTexture.begin(); iter != m_listAsyncTexture.end(); ++iter)
	{
		(*iter).get();
	}
	m_listAsyncTexture.clear();
}

_uint CLoading::Thread_Main(void * pArg)
{
	CLoading*		pLoading = reinterpret_cast<CLoading*>(pArg);

	_uint iFlag(0);

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_TEXTURE:
		iFlag = pLoading->Loading_For_Texture();
		break;
	case LOADING_STAGE:
		iFlag = pLoading->Loading_For_Stage();
		break;

	case LOADING_BOSS:
		break;

	}

	LeaveCriticalSection(pLoading->Get_Crt());

	//_endthreadex(0);

	return iFlag;
}
