#include "AStarMgr.h"

CAStarMgr::CAStarMgr()
{
}

CAStarMgr::~CAStarMgr()
{
	Free();
}

void CAStarMgr::AStar_Start(const _vec3& vStart, const _vec3& vGoal)
{
}

_int CAStarMgr::Get_TileIdx(const _vec3& vPos)
{
	return _int();
}

_bool CAStarMgr::Picking_Dot(const _vec3& vPos, const _int& iIndex)
{
	return _bool();
}

_bool CAStarMgr::Make_BestList(_int iStartIdx, _int iGoalIdx)
{
	return _bool();
}

_bool CAStarMgr::Make_Route(_int iStartIdx, _int iGoalIdx)
{
	return _bool();
}

_bool CAStarMgr::Check_Close(_int iIndex)
{
	return _bool();
}

_bool CAStarMgr::Check_Open(_int iIndex)
{
	return _bool();
}

void CAStarMgr::Free()
{
}
