#include "stdafx.h"
#include "JumpMonster.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

CJumpMonster::CJumpMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CJumpMonster::CJumpMonster(const CJumpMonster& rhs)
    : Base(rhs)
{
}

CJumpMonster::~CJumpMonster()
{
}

CJumpMonster* CJumpMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

HRESULT CJumpMonster::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CJumpMonster::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
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

    FadeInEnd = false;

    return S_OK;
}

_int CJumpMonster::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Update_InternalData();

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CJumpMonster::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CJumpMonster::Free()
{
    SUPER::Free();
}

void CJumpMonster::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CJumpMonster::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CJumpMonster::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CJumpMonster::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    // 해당 구역에 들어왔을 때 몬스터 여러마리 생성 (한번만)
}

void CJumpMonster::OnCollisionExited(CGameObject* pDst)
{

}

HRESULT CJumpMonster::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}

void CJumpMonster::Update_InternalData()
{
    //// 블랙보드 연결 대기, 안전 코드로 필수
    //if (!m_wpBlackBoard_Player.Get_BlackBoard())
    //{
    //    m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
    //    // 연결 실패
    //    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    //        return;
    //}

    //// 안전 코드를 거치면 일반 포인터로 접근 허용.
    //CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    //// 여기서부터 블랙보드의 정보를 얻어온다.
    //FadeInEnd = pBlackBoard->Get_SceneFade();

    //if (m_bIsTriggered)
    //    pBlackBoard->Get_SceneFade() = false;
}
