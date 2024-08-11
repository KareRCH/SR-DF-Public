#pragma once

#include "Base.h"
#include "BlackBoard.h"

BEGIN(Engine)

/// <summary>
/// 블랙보드 매니저
/// 블랙보드를 추가하고 블랙보드를 얻어와준다.
/// </summary>
class ENGINE_DLL CBlackBoardMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CBlackBoardMgr)
private:
	explicit CBlackBoardMgr();
	virtual ~CBlackBoardMgr();

private:
	virtual	void	Free();

public:
	HRESULT			Ready_BlackBoard();
	_int			Update_BlackBoard();

public:
	HRESULT			Add_BlackBoard(const wstring& strBoardName, CBlackBoard* pBlackBoard);
	HRESULT			Delete_BlackBoard(const wstring& strBoardName);
	CBlackBoard*	Get_BlackBoard(const wstring& strBoardName);
	

private:
	_unmap<wstring, CBlackBoard*>			m_mapBlackBoard;

};

END
