#include "stdafx.h"
#include "Trigger_Box.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "Brown.h"
#include "Gray.h"

CTrigger_Box::CTrigger_Box(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTrigger_Box::CTrigger_Box(const CTrigger_Box& rhs)
    : Base(rhs)
{
}

CTrigger_Box::~CTrigger_Box()
{
}

CTrigger_Box* CTrigger_Box::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("RectObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CTrigger_Box::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CTrigger_Box::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    if (!tObjectSerial.vecUserString.empty())
    {
        string strTrigger = tObjectSerial.vecUserString.front();
        if (strTrigger == "GateWay1")
        {
            m_fnTrigger = [this]() {
                this->GateWay();
            };
        }
        else if (strTrigger == "Turnnel")
        {
            m_fnTrigger = [this]() {
                this->Turnnel();
            };
        }
        else if (strTrigger == "Turnnel2")
        {
            m_fnTrigger = [this]() {
                this->Turnnel2();
            };
        }
        else if (strTrigger == "Entrance")
        {
            m_fnTrigger = [this]() {
                this->Entrance();
            };
        }
    }

    FadeInEnd = false;

    return S_OK;
}

_int CTrigger_Box::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);


    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CTrigger_Box::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTrigger_Box::Free()
{
    SUPER::Free();
}

void CTrigger_Box::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CTrigger_Box::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CTrigger_Box::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CTrigger_Box::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pDst);
    if (pPlayer)
    {
        if (m_fnTrigger)
        {
            m_fnTrigger();
        }
        Set_Dead();
    }
}

void CTrigger_Box::OnCollisionExited(CGameObject* pDst)
{

}

HRESULT CTrigger_Box::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}

void CTrigger_Box::GateWay()
{
    CBlackBoard_GateWay1* pBlackBoard = CBlackBoard_GateWay1::Create();
    Engine::Add_BlackBoard(L"GateWay1", pBlackBoard);

    CAceMonster* pMonster = nullptr;

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 5.f,
        m_pTransformComp->Get_Pos().z + 1.5f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);
    pBlackBoard->Get_VecMonster().push_back(pMonster);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 5.f,
        m_pTransformComp->Get_Pos().z - 1.5f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);
    pBlackBoard->Get_VecMonster().push_back(pMonster);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 5.f,
        m_pTransformComp->Get_Pos().z));
    pBlackBoard->Get_VecMonster().push_back(pMonster);
}

void CTrigger_Box::Turnnel()
{
    /*CBlackBoard_GateWay1* pBlackBoard = CBlackBoard_GateWay1::Create();
    Engine::Add_BlackBoard(L"GateWay1", pBlackBoard);*/

    CAceMonster* pMonster = nullptr;

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 1.5f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 3.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 4.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 6.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 8.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);
}

void CTrigger_Box::Turnnel2()
{
    Engine::Play_BGM(L"FallenAces", L"Music_ActionA #1667.wav", 0.9f);

    CAceMonster* pMonster = nullptr;

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 1.5f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 3.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 4.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 8.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 6.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    // ------------

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 3.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 3.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 4.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 2.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 5.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 1.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 3.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 3.5f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 4.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 2.5f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);
}




void CTrigger_Box::Entrance()
{
    Engine::Add_GameObject(L"UI", CUI_PlayerHurt::Create(m_pGraphicDev));

    Engine::Play_BGM(L"FallenAces", L"Music_ActionA #1667.wav", 0.9f);

    CAceMonster* pMonster = nullptr;

    // N

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 6.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 4.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 2.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 0.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 2.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 4.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 6.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    // W

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 6.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 4.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 2.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 0.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 2.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 4.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 6.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    // S

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 6.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 4.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 2.5f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 0.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 2.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 4.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 10.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z + 6.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    // E

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 6.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 4.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x - 2.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 0.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CGray::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 2.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CGray*>(pMonster)->Get_StateObj().Set_State(CGray::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 4.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);

    Add_GameObject(L"GameLogic", pMonster = CBrown::Create(m_pGraphicDev,
        m_pTransformComp->Get_Pos().x + 6.f,
        m_pTransformComp->Get_Pos().y - 1.f,
        m_pTransformComp->Get_Pos().z - 10.f));
    static_cast<CBrown*>(pMonster)->Get_StateObj().Set_State(CBrown::STATE_OBJ::CHASE);
}