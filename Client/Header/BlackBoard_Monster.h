#pragma once

#include "BlackBoard.h"
#include "GameObject.h"

class CBrown;
class CGray;
class CBoss;

/// <summary>
/// 이니셜라이즈용 구조체
/// </summary>

struct FBlackBoard_MonsterInit
{
	CBrown* pBrown;
	CGray* pGray;
	CBoss* pBoss;
};


/// <summary>
/// 플레이어 전용 블랙보드
/// 플레이어에게 필요한 정보라면 여기에 업데이트 하자
/// </summary>
class CBlackBoard_Monster : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Monster)

protected:
	explicit CBlackBoard_Monster();
	explicit CBlackBoard_Monster(const CBlackBoard_Monster& rhs);
	virtual ~CBlackBoard_Monster();

public:
	static CBlackBoard_Monster* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// 초기화용 구조체를 만들어 매개변수로 받게 한다.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_MonsterInit& tInit);



public:		// 정보만 얻을 때는 
	GETSET_EX1(CBrown*, m_pBrown, Brown, GET)
	GETSET_EX1(CGray*, m_pGray, Gray, GET)
	GETSET_EX1(CBoss*, m_pBoss, Boss, GET)

private:
	CBrown* m_pBrown = nullptr;
	CGray* m_pGray = nullptr;
	CBoss* m_pBoss = nullptr;

public:		// 데이터 가공의 성격을 띌때는 해당 함수를 직접 만들것.
	GETSET_EX2(_bool, m_bLightOn, ControlLight, GET, SET)
	GETSET_EX2(GAUGE<_float>, m_gBossHP, BossHP, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_gMonsterHP, MonsterHP, GET_C_REF, GET_REF)
	GETSET_EX2(CGameObject*, m_pHpOwner, HPOwner, SET_REF, GET_REF)
	GETSET_EX2(wstring, m_Owner, Owner, SET_REF, GET_REF)


private:
	_bool				m_bLightOn;
	GAUGE<_float>		m_gBossHP;	//보스hp ui에 저장용
	GAUGE<_float>		m_gMonsterHP;	// gray hp - ui에 저장용 
	CGameObject*		m_pHpOwner;
	wstring				m_Owner;

//	GETSET_EX2(CTextureComponent*, m_pRightHandComp, m_pRightHandComp, GET, SET)
};

