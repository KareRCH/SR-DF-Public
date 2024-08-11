#include "Scene_Parsed.h"

#include "Terrain.h"

#include "Serialize_BaseClass.h"

#include "Export_System.h"
#include "Export_Utility.h"

CScene_Parsed::CScene_Parsed(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CScene_Parsed::~CScene_Parsed()
{
}

CScene_Parsed* CScene_Parsed::Create(LPDIRECT3DDEVICE9 pGraphicDev, const char* pSceneFileName)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene(pSceneFileName)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Stage Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CScene_Parsed::Free()
{
    SUPER::Free();
}

HRESULT CScene_Parsed::Ready_Scene()
{
    FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);
    
    return S_OK;
}

HRESULT CScene_Parsed::Ready_Scene(const char* pSceneFileName)
{
    FSerialize_Scene tSceneSerial;

    // 파싱 데이터 로드
    //string strFileName = pSceneFileName;
    //string strJson;
    //ifstream ifs("Resource/Data/Scene/" + strFileName + ".ascene");
    //if (ifs.is_open())
    //{
    //    strJson = string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
    //    ifs.close();
    //    cout << "씬 파일 불러옴!" << endl;

    //    if (tSceneSerial.Receive_ByRapidJSON(strJson))
    //    {
    //        for (size_t i = 0; i < tSceneSerial.vecLayer.size(); i++)
    //        {
    //            FSerialize_Layer& tLayerSerial = tSceneSerial.vecLayer[i];

    //            CLayer* pLayer = CLayer::Create(tLayerSerial.fPriority);
    //            wstring strConvert(tLayerSerial.tHeader.strName.begin(), tLayerSerial.tHeader.strName.end());
    //            m_mapLayer.emplace(strConvert, pLayer);

    //            for (size_t j = 0; j < tLayerSerial.vecGameObject.size(); j++)
    //            {
    //                FSerialize_GameObject& tObjectSerial = tLayerSerial.vecGameObject[j];
    //                
    //                wstring strConvertObject(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());

    //                pLayer->Add_GameObject(strConvertObject, 
    //                    CAbsFac_GameObject::Create(m_pGraphicDev, tObjectSerial));
    //            }

    //            if (i == 0)
    //            {
    //                // 터레인 로드
    //                pLayer->Add_GameObject(CTerrain::Create(m_pGraphicDev, tSceneSerial.refTerrainName.c_str()));
    //            }
    //        }
    //    }
    //}
    //else
    //    cout << "씬 로드 불가" << endl;


    FAILED_CHECK_RETURN(Ready_Scene(), E_FAIL);

    return S_OK;
}

_int CScene_Parsed::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_Parsed::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();
}

void CScene_Parsed::Render_Scene()
{
    // DEBUG 용
}

HRESULT CScene_Parsed::ReadyLate_Scene()
{

    return S_OK;
}

HRESULT CScene_Parsed::Ready_Layer()
{


    return S_OK;
}

HRESULT CScene_Parsed::Ready_Layer_Completed()
{
    /*Add_GameObject(L"Camera", CDynamicCamera::Create(m_pGraphicDev,
        &_vec3(0.f, 5.f, -10.f),
        &_vec3(0.f, 0.f, 1.f),
        &_vec3(0.f, 1.f, 0.f)));

    Add_GameObject(L"UI", CUI_Player::Create(m_pGraphicDev));

    Add_GameObject(L"GameLogic", CPlayer::Create(m_pGraphicDev));*/

    return S_OK;
}
