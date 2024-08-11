#include "stdafx.h"
#include "BlackBoard_GateWay1.h"

CBlackBoard_GateWay1::CBlackBoard_GateWay1()
	: Base()
{
}

CBlackBoard_GateWay1::CBlackBoard_GateWay1(const CBlackBoard_GateWay1& rhs)
	: Base(rhs)
{
}

CBlackBoard_GateWay1::~CBlackBoard_GateWay1()
{
	Free();
}

CBlackBoard_GateWay1* CBlackBoard_GateWay1::Create()
{
    ThisClass* pInstance = new ThisClass();

    if (FAILED(pInstance->Ready_BlackBoard()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BlackBoard_GateWay Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CBlackBoard_GateWay1::Free()
{
}

void CBlackBoard_GateWay1::Add_Monster(CAceMonster* const pMonster)
{
    m_vecMonster.push_back(pMonster);
}

_bool CBlackBoard_GateWay1::Is_MonsterAllDead()
{
    for (auto iter = m_vecMonster.begin(); iter != m_vecMonster.end(); ++iter)
    {
        if (!(*iter)->Get_IsMonsterDeath())
            return false;
    }

    return true;
}
