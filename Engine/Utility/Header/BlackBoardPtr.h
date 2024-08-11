#pragma once

#include "Base.h"
#include "BlackBoard.h"

BEGIN(Engine)

/// <summary>
/// �����带 �����ϴ� ��ũ ������ ������ ��ü
/// ���� RefCount�� ������ �ʴ´�.
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
		// �̹� ����� ���� �����ڵ� �۵��� ���ϴ� �ߺ� �����ص� ���� ����
		// RefCount ����
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
		// ������ �������� ������ �����Ų��.
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
			// ����� ������� �ڵ����� nullptr�� ä���ش�.
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