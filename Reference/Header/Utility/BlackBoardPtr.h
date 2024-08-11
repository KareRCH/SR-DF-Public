#pragma once

#include "Base.h"
#include "BlackBoard.h"

BEGIN(Engine)

/// <summary>
/// 블랙보드를 참조하는 위크 포인터 개념의 객체
/// 따로 RefCount를 가지지 않는다.
/// </summary>
template <typename T,
	typename = enable_if_t<is_base_of_v<CBlackBoard, T>>>
class FBlackBoardPtr
{
public:
	explicit FBlackBoardPtr()
		: m_pBlackBoard(nullptr)
	{}
	explicit FBlackBoardPtr(const FBlackBoardPtr<T>& rhs) {}
	~FBlackBoardPtr()
	{
		// 이미 만료된 것은 해제코드 작동을 안하니 중복 해제해도 문제 없음
		// RefCount 감소
		Safe_Release(m_pBlackBoard);
	}

public:
	void Set_BlackBoard(CBlackBoard* const pBlackBoard)
	{
		if (!pBlackBoard)
			return;

		if (pBlackBoard->IsExpired())
			return;

		m_pBlackBoard = pBlackBoard;
		// 설정된 블랙보드의 수명을 연장시킨다.
		pBlackBoard->AddRef();
	}

public:
	T* Get_BlackBoard()
	{
		if (m_pBlackBoard == nullptr)
			return nullptr;

		if (m_pBlackBoard->IsExpired())
		{
			Safe_Release(m_pBlackBoard);
			// 만료된 블랙보드는 자동으로 nullptr를 채워준다.
			return static_cast<T*>(m_pBlackBoard = nullptr);
		}

		return static_cast<T*>(m_pBlackBoard);
	}

public:
	_bool		IsAvailable() 
	{ 
		if (!m_pBlackBoard)
			return nullptr;
		return  m_pBlackBoard->IsExpired(); 
	}

private:
	CBlackBoard* m_pBlackBoard;
};

END