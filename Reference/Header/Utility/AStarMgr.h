#pragma once

#include "Base.h"
#include "Engine_Define.h"

class CAStarMgr : public CBase
{	
private:
	explicit CAStarMgr();
	virtual ~CAStarMgr();

public:
	void		AStar_Start(const _vec3& vStart, const _vec3& vGoal);
	_int		Get_TileIdx(const _vec3& vPos);
	_bool		Picking_Dot(const _vec3& vPos, const _int& iIndex);

private:
	_bool		Make_BestList(_int iStartIdx, _int iGoalIdx);
	_bool		Make_Route(_int iStartIdx, _int iGoalIdx);
	_bool		Check_Close(_int iIndex);
	_bool		Check_Open(_int iIndex);

public:
	_int				m_iStartIdx = 0;
	//list<int>		m_CloseList;
	//list<int>		m_OpenList;
	//list<TILE*>		m_BestList;
	//list<TILE*>& Get_BestList() { return m_BestList; }

private:
	virtual		void	Free();
};

