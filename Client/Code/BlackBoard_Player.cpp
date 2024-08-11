#include "stdafx.h"
#include "BlackBoard_Player.h"

#include "Player.h"

CBlackBoard_Player::CBlackBoard_Player()
    : Base()
    , m_fHP(GAUGE<_float>(100.f)), m_fStamina(GAUGE<_float>(100.f)), m_bGunLight(false)
    , m_bPlayerEquipGunState(false), m_bPlayerAttackState(false), m_bPlayerPickUpState(false), m_bPlayerDetect(false) // 이줄은 크로스헤어용
    , m_bPlayerHitState(false)
    , m_bFade(false), m_bMagicBottle(false)
{
}

CBlackBoard_Player::CBlackBoard_Player(const CBlackBoard_Player& rhs)
    : Base(rhs)
{
}

CBlackBoard_Player::~CBlackBoard_Player()
{
    Free();
}

CBlackBoard_Player* CBlackBoard_Player::Create()
{
    ThisClass* pInstance = new ThisClass();

    if (FAILED(pInstance->Ready_BlackBoard()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BlackBoard_Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CBlackBoard_Player::Free()
{
    
}

HRESULT CBlackBoard_Player::Ready_BlackBoard(const FBlackBoard_PlayerInit& tInit)
{
    m_pPlayer = tInit.pPlayer;



    return S_OK;
}
