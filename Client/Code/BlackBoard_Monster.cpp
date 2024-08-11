#include "stdafx.h"
#include "BlackBoard_Monster.h"
#include "AceBoss.h"
#include "Brown.h"
#include "Gray.h"

CBlackBoard_Monster::CBlackBoard_Monster()
	: Base()
    , m_bLightOn(FALSE)
{
    
}

CBlackBoard_Monster::CBlackBoard_Monster(const CBlackBoard_Monster& rhs)
	: Base(rhs)
{
}

CBlackBoard_Monster::~CBlackBoard_Monster()
{
	Free();
}

CBlackBoard_Monster* CBlackBoard_Monster::Create()
{
    ThisClass* pInstance = new ThisClass();

    if (FAILED(pInstance->Ready_BlackBoard()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BlackBoard_Monster Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CBlackBoard_Monster::Free()
{
    //여기서 뭐 release할거 알아서 추가하셈 
}

HRESULT CBlackBoard_Monster::Ready_BlackBoard(const FBlackBoard_MonsterInit& tInit)
{
    m_gMonsterHP.Cur = 0.f;
    m_gMonsterHP.Max = 100.f;
    
    return S_OK;
}
