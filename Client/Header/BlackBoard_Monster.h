#pragma once

#include "BlackBoard.h"
#include "GameObject.h"

class CBrown;
class CGray;
class CBoss;

/// <summary>
/// �̴ϼȶ������ ����ü
/// </summary>

struct FBlackBoard_MonsterInit
{
	CBrown* pBrown;
	CGray* pGray;
	CBoss* pBoss;
};


/// <summary>
/// �÷��̾� ���� ������
/// �÷��̾�� �ʿ��� ������� ���⿡ ������Ʈ ����
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
	// �ʱ�ȭ�� ����ü�� ����� �Ű������� �ް� �Ѵ�.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_MonsterInit& tInit);



public:		// ������ ���� ���� 
	GETSET_EX1(CBrown*, m_pBrown, Brown, GET)
	GETSET_EX1(CGray*, m_pGray, Gray, GET)
	GETSET_EX1(CBoss*, m_pBoss, Boss, GET)

private:
	CBrown* m_pBrown = nullptr;
	CGray* m_pGray = nullptr;
	CBoss* m_pBoss = nullptr;

public:		// ������ ������ ������ �鶧�� �ش� �Լ��� ���� �����.
	GETSET_EX2(_bool, m_bLightOn, ControlLight, GET, SET)
	GETSET_EX2(GAUGE<_float>, m_gBossHP, BossHP, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_gMonsterHP, MonsterHP, GET_C_REF, GET_REF)
	GETSET_EX2(CGameObject*, m_pHpOwner, HPOwner, SET_REF, GET_REF)
	GETSET_EX2(wstring, m_Owner, Owner, SET_REF, GET_REF)


private:
	_bool				m_bLightOn;
	GAUGE<_float>		m_gBossHP;	//����hp ui�� �����
	GAUGE<_float>		m_gMonsterHP;	// gray hp - ui�� ����� 
	CGameObject*		m_pHpOwner;
	wstring				m_Owner;

//	GETSET_EX2(CTextureComponent*, m_pRightHandComp, m_pRightHandComp, GET, SET)
};

