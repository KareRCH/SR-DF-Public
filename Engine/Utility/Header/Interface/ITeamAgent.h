#pragma once

#include "Engine_Define.h"


BEGIN(Engine)

/// <summary>
/// ���� �ĺ��� �� �ְ� �����ϴ� �������̽�
/// �������̽������� �ظ��� �͵��� ������ �س��� ����.
/// </summary>
class ENGINE_DLL ITeamAgent abstract
{
public:
	static const BYTE TEAM_END = (BYTE)255U;		// ������ �� ID

	enum class ERELATION
	{
		NONE,
		HOSTILE,
		NUETRAL,
		FRIEND
	};

public:
	ITeamAgent() : m_ulPrivID(InitID()) {}
	ITeamAgent(const ITeamAgent& _rhs) = delete;
	virtual ~ITeamAgent() {}

private:
	static ULONGLONG ID_COUNT;								// ���� ID ������
	static map<BYTE, map<BYTE, ERELATION>>	m_mapTeamRelation;			// ���� ����, ���赵�� ���� ��. �߸����� ó����.
	
private:
	BYTE						m_bTeamID = (BYTE)0U;		// �ڽ��� ��ID

	ULONGLONG					m_ulPrivID;					// �ڽ��� ID
	map<ULONGLONG, ERELATION>	m_mapPrivRelation;			// ���� ����, �켱�� ����

public:
	const int Get_TeamID() const { return static_cast<int>(m_bTeamID); }
	void Set_TeamID(const int value) { m_bTeamID = static_cast<BYTE>(value); }

private:
	// ��ü�� �ʱ�ȭ �� ������ ID�� ������Ŵ, �����ؼ� ������ �ϴ� ���, ��� ���� ULONGLONG�� ũ�ϱ� ��
	ULONGLONG InitID() { return ID_COUNT++; }

public:
	static void Add_TeamRelation(int iSrcTeam, int iDstTeam, ERELATION eRelation)
	{
		auto iter = m_mapTeamRelation.find(static_cast<BYTE>(iSrcTeam));
		if (iter != m_mapTeamRelation.end())
		{
			m_mapTeamRelation[static_cast<BYTE>(iSrcTeam)].emplace(static_cast<BYTE>(iDstTeam), eRelation);
		}
		else
		{
			map<BYTE, ERELATION> mapTeamRelation = map<BYTE, ERELATION>();
			mapTeamRelation.emplace(static_cast<BYTE>(iDstTeam), eRelation);
			m_mapTeamRelation.emplace(static_cast<BYTE>(iSrcTeam), mapTeamRelation);
		}
	}

	// Dst�� ���� ���� ��ȣ���� üũ�Ѵ�.
	static ERELATION Check_TeamRelation(ITeamAgent* pDst, ITeamAgent* pSrc)
	{
		ERELATION tRelation = m_mapTeamRelation[pSrc->m_bTeamID][pDst->m_bTeamID];
		if (tRelation == ERELATION::NONE)
			return ERELATION::NUETRAL;
		return tRelation;
	}

	// Src�� ���赵�� ���� Dst�� ���� ��ȣ���� üũ�Ѵ�.
	static ERELATION Check_PrivRelation(ITeamAgent* pDst, ITeamAgent* pSrc)
	{
		ERELATION tRelation = pSrc->m_mapPrivRelation[pDst->m_ulPrivID];

		if (tRelation == ERELATION::NONE)
			return ERELATION::NUETRAL;

		return tRelation;
	}

	// ����, �� ������ ��ȣ���� �����Ѵ�.
	static ERELATION Check_Relation(ITeamAgent* pDst, ITeamAgent* pSrc)
	{
		ERELATION tRelation = pSrc->m_mapPrivRelation[pDst->m_ulPrivID];

		if (tRelation == ERELATION::NONE)
			return Check_TeamRelation(pDst, pSrc);

		return tRelation;
	}
};

using ERELATION_STATE = ITeamAgent::ERELATION;


END

