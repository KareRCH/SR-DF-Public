#include "stdafx.h"
#include "JumpMainBgm.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

CJumpMainBgm::CJumpMainBgm(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CJumpMainBgm::CJumpMainBgm(const CJumpMainBgm& rhs)
    : Base(rhs)
{
}

CJumpMainBgm::~CJumpMainBgm()
{
}

CJumpMainBgm* CJumpMainBgm::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

HRESULT CJumpMainBgm::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CJumpMainBgm::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
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

_int CJumpMainBgm::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Update_InternalData();

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CJumpMainBgm::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CJumpMainBgm::Free()
{
    SUPER::Free();
}

void CJumpMainBgm::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CJumpMainBgm::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CJumpMainBgm::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CJumpMainBgm::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    // 최초 한번만 재생 (이후에 다시 들어와도 재생되지 않게)
    if (!m_bSoundOff)
    {
        Engine::StopAll();
        Engine::Play_BGM(L"FallenAces", L"Prologue.mp3", 0.75f);

        m_bSoundOff = true;
    }
}

void CJumpMainBgm::OnCollisionExited(CGameObject* pDst)
{

}

HRESULT CJumpMainBgm::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}

void CJumpMainBgm::Update_InternalData()
{

}
