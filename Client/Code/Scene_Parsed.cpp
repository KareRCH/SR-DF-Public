#include "stdafx.h"
#include "Scene_Parsed.h"

#include "AbsFac_GameObject.h"
#include "Terrain.h"
#include "Player.h"

#include "Serialize_BaseClass.h"
#include <DynamicCamera.h>
#include <UI_Player.h>
#include <SkyBox.h>
#include "AceBuilding.h"
#include "UI_CrossHair.h"
#include "UI_PlayerFace.h"
#include "PlayerLighter.h"
#include "PlayerGunLighter.h"
#include "SnowParticle.h"

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

    // ∆ƒΩÃ µ•¿Ã≈Õ ∑ŒµÂ
    string strFileName = pSceneFileName;
    string strJson;
    ifstream ifs("Resource/Data/Scene/" + strFileName + ".ascene");
    if (ifs.is_open())
    {
        strJson = string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
        ifs.close();
        cout << "æ¿ ∆ƒ¿œ ∫“∑Øø»!" << endl;

        if (tSceneSerial.Receive_ByRapidJSON(strJson))
        {
            for (size_t i = 0; i < tSceneSerial.vecLayer.size(); i++)
            {
                FSerialize_Layer& tLayerSerial = tSceneSerial.vecLayer[i];

                CLayer* pLayer = CLayer::Create(tLayerSerial.fPriority);
                wstring strConvert(tLayerSerial.tHeader.strName.begin(), tLayerSerial.tHeader.strName.end());
                m_mapLayer.emplace(strConvert, pLayer);

                for (size_t j = 0; j < tLayerSerial.vecGameObject.size(); j++)
                {
                    FSerialize_GameObject& tObjectSerial = tLayerSerial.vecGameObject[j];
                    
                    wstring strConvertObject(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());

                    CGameObject* pGameObject = CAbsFac_GameObject::Create(m_pGraphicDev, tObjectSerial);
                    if (pGameObject != nullptr)
                        pLayer->Add_GameObject(strConvertObject, pGameObject);
                }
            }
            CLayer* pLayer = CLayer::Create(0.f);
            m_mapLayer.emplace(L"Terrain", pLayer);
            pLayer->Add_GameObject(L"Terrain", CTerrain::Create(m_pGraphicDev, tSceneSerial.refTerrainName.c_str()));
        }
    }
    else
        cout << "æ¿ ∑ŒµÂ ∫“∞°" << endl;


    FAILED_CHECK_RETURN(Ready_Scene(), E_FAIL);

    SoundReady(strFileName);
    
    if(strFileName.compare("SeongHee") == 0)
        Add_GameObject(L"Environment", L"SnowParticle", CSnowParticle::Create(m_pGraphicDev, {40.f, 3.f, 40.f}, 10000));
    
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
    // DEBUG øÎ
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
    
    Add_GameObject(L"Camera", CDynamicCamera::Create(m_pGraphicDev,
        &_vec3(0.f, 5.f, -10.f),
        &_vec3(0.f, 0.f, 1.f),
        &_vec3(0.f, 1.f, 0.f)));

    Add_GameObject(L"UI", CUI_Player::Create(m_pGraphicDev));

    //Add_GameObject(L"GameLogic", CPlayer::Create(m_pGraphicDev));

    Add_GameObject(L"Environment", L"SkyBox", CSkyBox::Create(m_pGraphicDev, 10.f));

    Add_GameObject(L"UI", L"UI_CrossHair", CUI_CrossHair::Create(m_pGraphicDev));
    Add_GameObject(L"UI", L"UI_PlayerFace", CUI_PlayerFace::Create(m_pGraphicDev));

    Add_GameObject(L"GameLogic", L"PlayerLighter", CPlayerLighter::Create(m_pGraphicDev));
    Add_GameObject(L"GameLogic", L"PlayerGunLighter", CPlayerGunLighter::Create(m_pGraphicDev));
    
    return S_OK;
}

void CScene_Parsed::SoundReady(string _scene)
{
    if (_scene.compare("Stage1") == 0) 
    {
        Engine::StopAll();
        Engine::Play_BGM(L"FallenAces", L"Ambience_Wind.wav", 0.5f);
    }
    else if (_scene.compare("BossStage") == 0)
    {
        Engine::StopAll();
        Engine::Play_BGM(L"FallenAces", L"Scene_BossStage.wav", 0.5f);
    }
    else if (_scene.compare("Malone") == 0)
    {
        Engine::StopAll();
        Engine::Play_BGM(L"FallenAces", L"Scene_Malone.wav", 0.5f);
    }
    else if (_scene.compare("SeongHee") == 0)
    {
        Engine::StopAll();
        Engine::Play_BGM(L"FallenAces", L"Ambience_Wind.wav", 0.5f);

    }

}
