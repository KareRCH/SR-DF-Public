#include "Scene_AnimationTool.h"

#include "BackGround.h"
#include "MyTexture.h"

#include "Export_System.h"
#include "Export_Utility.h"

CScene_AnimationTool::CScene_AnimationTool(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{
}

CScene_AnimationTool::~CScene_AnimationTool()
{
}

CScene_AnimationTool* CScene_AnimationTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene()))
    {
        Safe_Release(pInstance);
        MSG_BOX("MapTool Scene Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CScene_AnimationTool::Free()
{
    SUPER::Free();
}

HRESULT CScene_AnimationTool::Ready_Scene()
{
    FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

    return S_OK;
}

_int CScene_AnimationTool::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_AnimationTool::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();


}

void CScene_AnimationTool::Render_Scene()
{
    SUPER::Render_Scene();
    // 디버그

}

HRESULT CScene_AnimationTool::Ready_Prototype()
{
//	// 로고용 텍스처들
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/LogoShot.png", TEX_NORMAL, L"UI", L"Logo"), E_FAIL);
//	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RightHand1.png", TEX_NORMAL, L"UI", L"RightHand"), E_FAIL);
//
//#pragma region 싱글 텍스처
//	// 왼손 달리기
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RunHand/LeftHand_Run (%d).png", TEX_NORMAL, L"Player", L"Left_RunHand", _range<_uint>(0U, 0U)), E_FAIL);
//	// 오른손 달리기
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RunHand/RightHand_Run (%d).png", TEX_NORMAL, L"Player", L"Right_RunHand", _range<_uint>(0U, 0U)), E_FAIL);
//	// 오픈 핸드 [오른손이 무언가를 할 때 아무것도 아닌상태] (왼손)
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/OpenHand/OpenHand (%d).png", TEX_NORMAL, L"Player", L"OpenHand", _range<_uint>(0U, 0U)), E_FAIL);
//#pragma endregion
//
//#pragma region 멀티 텍스처
//	// 왼손
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/LeftHand/LeftHand (%d).png", TEX_NORMAL, L"Player", L"Left_Hand", _range<_uint>(0U, 1U)), E_FAIL);
//	// 오른손
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RightHand/RightHand (%d).png", TEX_NORMAL, L"Player", L"Right_Hand", _range<_uint>(0U, 1U)), E_FAIL);
//	// 주먹 차징 (오른손)
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RightHand_Charging/RightHand_Charging (%d).png", TEX_NORMAL, L"Player", L"RightHand_Charging", _range<_uint>(0U, 2U)), E_FAIL);
//	// 버리기 (오른손)
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/ThrowHand/UnderThrowRightHand (%d).png", TEX_NORMAL, L"Player", L"UnderThrow_RightHand", _range<_uint>(0U, 0U)), E_FAIL);
//	// 쇠파이프
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/Steel_Pipe/Steel_Pipe (%d).png", TEX_NORMAL, L"Player", L"Steel_Pipe", _range<_uint>(0U, 3U)), E_FAIL);
//	// 쇠파이프 차징
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/Steel_Pipe_Charging/Steel_Pipe_Charging (%d).png", TEX_NORMAL, L"Player", L"Steel_Pipe_Charging", _range<_uint>(0U, 3U)), E_FAIL);
//	// 프라이팬
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/FryingPan/FryingPan (%d).png", TEX_NORMAL, L"Player", L"FryingPan", _range<_uint>(0U, 3U)), E_FAIL);
//	// 프라이팬 차징
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/FryingPan_Charging/FryingPan_Charging (%d).png", TEX_NORMAL, L"Player", L"FryingPan_Charging", _range<_uint>(0U, 3U)), E_FAIL);
//	// 라이터
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/Zippo/Zippo (%d).png", TEX_NORMAL, L"Player", L"Righter", _range<_uint>(0U, 5U)), E_FAIL);
//	// 플레이어 총
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/Gun/Gun (%d).png", TEX_NORMAL, L"Player", L"Gun", _range<_uint>(0U, 3U)), E_FAIL);
//	// 플레이어 총 회전
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/GunSpin/Spin (%d).png", TEX_NORMAL, L"Player", L"Gun_Spin", _range<_uint>(0U, 3U)), E_FAIL);
//	// 플레이어 톰슨 기관총
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/Thompson/Thompson (%d).png", TEX_NORMAL, L"Player", L"Thompson", _range<_uint>(0U, 3U)), E_FAIL);
//	// 맥주병
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/BeerBottle/BeerBottle (%d).png", TEX_NORMAL, L"Player", L"BeerBottle", _range<_uint>(0U, 4U)), E_FAIL);
//	// 발차기
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/PlayerKick/PlayerKick (%d).png", TEX_NORMAL, L"Player", L"Kick", _range<_uint>(0U, 2U)), E_FAIL);
//#pragma endregion
//
//	// 배경 사이즈 (클라이언트 비율에 맞는 텍스처 찾기)
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/MAP/ClientSizeBox1280x720.png", TEX_NORMAL, L"UI", L"ClientSize"), E_FAIL);
//
//	// 플레이어 오른손
//	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RightHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Right_Hand")), E_FAIL);
//
//	// 플레이어 왼손
//	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeftHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player", L"Left_Hand")), E_FAIL);
//
//
//	// 사이즈 표시 텍스처
//	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ClientTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"ClientSize")), E_FAIL);	// 텍스처
//
//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/Title.png", TEX_NORMAL, L"UI", L"Title"), E_FAIL);

	//#pragma region Brown Monster - 작업대기상태 
	//	//// Monster Texture - Brown - Single : 3EA
	//	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Stand/Stand_South.png", TEX_NORMAL, L"Brown_Single", L"Stand_South"), E_FAIL);
	//	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Single/Suspicious.png", TEX_NORMAL, L"Brown_Single", L"Suspicious"), E_FAIL);
	//	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Single/CrotchHit.png", TEX_NORMAL, L"Brown_Single", L"CrotchHit"), E_FAIL);
	//
	//	// Monster Texture - Brown - Multi : 18EA
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/BasicAttack/BasicAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"BasicAttack", _range<_uint>(0U, 4U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/InchForward/InchForward_%d.png", TEX_NORMAL, L"Brown_Multi", L"InchForward", _range<_uint>(0U, 5U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"HeavyAttack", _range<_uint>(0U, 6U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Chopped/KarateChopped_%d.png", TEX_NORMAL, L"Brown_Multi", L"Chopped", _range<_uint>(0U, 17U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Brown_Multi", L"FacePunch", _range<_uint>(0U, 7U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Run/South/RunSouth_%d.png", TEX_NORMAL, L"Brown_Multi", L"RunSouth", _range<_uint>(0U, 20U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Headless/Headless_%d.png", TEX_NORMAL, L"Brown_Multi", L"Headless", _range<_uint>(0U, 15U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Walk/South/Walk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_South", _range<_uint>(0U, 23U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Strafing/Strafing_%d.png", TEX_NORMAL, L"Brown_Multi", L"Strafing", _range<_uint>(0U, 5U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Falling/Falling_%d.png", TEX_NORMAL, L"Brown_Multi", L"Falling", _range<_uint>(0U, 11U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Pooping/Pooping_%d.png", TEX_NORMAL, L"Brown_Multi", L"Pooping", _range<_uint>(0U, 2U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Rest/IdleReady_%d.png", TEX_NORMAL, L"Brown_Multi", L"Rest", _range<_uint>(0U, 6U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Dazed/Dazed_%d.png", TEX_NORMAL, L"Brown_Multi", L"Dazed", _range<_uint>(0U, 13U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Taunt/Taunt_%d.png", TEX_NORMAL, L"Brown_Multi", L"Taunt", _range<_uint>(0U, 5U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/GetUp/Getup_%d.png", TEX_NORMAL, L"Brown_Multi", L"GetUp", _range<_uint>(0U, 5U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Death/Death_%d.png", TEX_NORMAL, L"Brown_Multi", L"Death", _range<_uint>(0U, 4U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Jump/Jump_%d.png", TEX_NORMAL, L"Brown_Multi", L"Jump", _range<_uint>(0U, 6U)), E_FAIL);
	//	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Hit/Hit_%d.png", TEX_NORMAL, L"Brown_Multi", L"Hit", _range<_uint>(0U, 5U)), E_FAIL);
	//#pragma endregion

		// 프로토타입 인스턴스를 등록한다.
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// 트랜스폼
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// 버퍼
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// 텍스처
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"RightHand")), E_FAIL);	// 텍스처

	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Brown_Single", L"Suspicious")), E_FAIL);	// Brown몬스터 

	return S_OK;
}

HRESULT CScene_AnimationTool::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// 정상
	// 레이어 초기화
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create(0.f)), E_FAIL);

	// 배경생성
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", CBackGround::Create(m_pGraphicDev)), E_FAIL);

	// 플레이어 텍스처 생성
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MyTexture", CMyTexture::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}
