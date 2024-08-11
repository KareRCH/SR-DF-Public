#pragma once

#include "BlackBoard.h"
#include <AceMonster.h>

class CBlackBoard_GateWay1 : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_GateWay1)

protected:
	explicit CBlackBoard_GateWay1();
	explicit CBlackBoard_GateWay1(const CBlackBoard_GateWay1& rhs);
	virtual ~CBlackBoard_GateWay1();

public:
	static CBlackBoard_GateWay1* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }

public:
	void Add_Monster(CAceMonster* const pMonster);
	_bool Is_MonsterAllDead();

public:
	GETSET_EX1(vector<CAceMonster*>, m_vecMonster, VecMonster, GET_REF)

protected:
	vector<CAceMonster*>	m_vecMonster;
	_bool					m_bIsTriggered = false;		// 페이즈 들어갔는지 체크
};

