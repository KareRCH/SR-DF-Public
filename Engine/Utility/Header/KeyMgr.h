#pragma once

#include "Base.h"
#include "Engine_Define.h"

#include "InputDev.h"

BEGIN(Engine)

#define MAX_DINPUT_KEY 256
#define MAX_DINPUT_MOUSE DIM_END

/// <summary>
/// Ű ���¿� ���� ����ü
/// </summary>
typedef enum class _EINPUT_KEY_STATE : int
{
	RELEASED,       // ��
	NUETRAL,        // �߸�
	PRESSING,       // ����
	PRESSED,        // �ѹ��� ����
}EINPUT_KEY_STATE, EINPUT_MOUSE_STATE;

/// <summary>
/// �Է� �����͸� �����ϴ� ����ü
/// </summary>
typedef struct _INPUT_KEY_INFO
{
	const TCHAR* const						sKeyName;			// ����Ű �̸�, �� �̸��� ���� Ű�� Ž��
	vector<tuple<int, EINPUT_KEY_STATE>>	vKeys;				// ���� Ű �Է� ����, ���� ����

public:
	_INPUT_KEY_INFO(const TCHAR* sKeyName)
		: sKeyName(sKeyName)
	{

	}

	template<typename T>
	void Add_Key(T _iKey)
	{
		// �ߺ� üũ
		if (!vKeys.empty())
		{
			bool bExists = true;
			for_each(vKeys.begin(), vKeys.end(), [key = _iKey, &bExists](const tuple<int, EINPUT_KEY_STATE>& _tKey) {
				if (key == get<0>(_tKey))
					bExists = false;
				});

			// �̹� �ִ� Ű�� �������� �ʴ´�.
			if (!bExists)
				return;
		}
		else
			vKeys.push_back(make_tuple(_iKey, EINPUT_KEY_STATE::NUETRAL));
	}

	template<typename T>
	void Add_Keys(T _iKey)
	{
		Add_Key(_iKey);
	}

	// ������ �߰��� �� ����
	template<typename T, typename... Args>
	void Add_Keys(T _iKey, Args... _iKeys)
	{
		Add_Key(_iKey);
		Add_Keys(_iKeys...);
	}


}INPUT_KEY_INFO;

/// <summary>
/// ��ǲ ����̽��� ���ӵ� Ű �Ŵ���
/// Ű �Ŵ����� �ܼ� Ű�Է��� Down, Up, Pressing ���¸� üũ�Ͽ� �˷��ֱ⵵ ������.
/// ���ӿ� �Է¿� �־� Ư���� �ൿ�� Ű �Է¿� ���� ó���ϰ�
/// ���۹� ����� ���� ��ɵ鿡 �����ϵ��� �����ִ� ���ҵ� �մϴ�.
/// ��Ű, ����Ű�� ���� ��ɵ鵵 �����ϵ��� ������� �Ŵ����Դϴ�.
/// </summary>
class ENGINE_DLL CKeyMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CKeyMgr)

private:
	explicit CKeyMgr();
	virtual ~CKeyMgr();

public:
	virtual void	Free();

public:
	HRESULT			Ready_Key();
	void			Update_Key();
	// LateUpdate�� ��ġ�� ��.
	void			LateUpdate_Key();

public:
	// �ܼ�Ű �Է�üũ �Լ�
	inline bool		Key_Pressing(const int& iKey);
	inline bool		Key_Down(const int& iKey);
	inline bool		Key_Up(const int& iKey);

	// �ܼ�Ű �Է�üũ �Լ�
	inline bool		Mouse_Pressing(const MOUSEKEYSTATE& iMouse);
	inline bool		Mouse_Down(const MOUSEKEYSTATE& iMouse);
	inline bool		Mouse_Up(const MOUSEKEYSTATE& iMouse);

private:
	// �Ϲ����� �Է¿� ���� �����ϱ� ���� �뵵�� ���Դϴ�.
	bool				m_bKeyState[MAX_DINPUT_KEY];
	bool				m_bMouseState[MAX_DINPUT_MOUSE];
	// �Է�Ű�� ���� ������ �����Ѵ�.
	using input_key_map = _unmap<const _tchar*, vector<INPUT_KEY_INFO*>>;
	input_key_map m_mapInputKeys;

public:
	void Add_InputKey(INPUT_KEY_INFO* pInputKey);
	void Remove_InputKey(INPUT_KEY_INFO* pInputKey);

public:
	// �ش� �̸��� ���� Ű���� �о� ���ۿ� ����ϱ� ���� �Է�Ű ����ü�� �б������ ��ȯ�޴� �ζ��� �Լ�
	const INPUT_KEY_INFO* Get_InputKey(const TCHAR* sKeyName) const
	{
		/*if (m_vInputKeys.empty())
			return nullptr;

		for (int i = 0; i < m_vInputKeys.size(); ++i)
		{
			if (0 == lstrcmp(m_vInputKeys[i]->sKeyName, sKeyName))
			{
				return m_vInputKeys[i];
			}
		}*/

		return nullptr;
	}
	const INPUT_KEY_INFO* operator [](const TCHAR* sKeyName) const
	{
		return Get_InputKey(sKeyName);
	}

	// Get_InputKey�� �����Ͽ�, �������� �б� ���� Ű ����ü�� ����, �Է�Ű�� ���� ���¸� üũ�մϴ�.
	// Pressed ���� Ȯ�ο�
	const bool IsKeyPressed(const INPUT_KEY_INFO* pKey) const
	{
		if (nullptr == pKey)
			return false;

		for (auto iter = pKey->vKeys.begin(); iter != pKey->vKeys.end(); ++iter)
		{
			if (EINPUT_KEY_STATE::PRESSED == get<1>(*iter))
				return true;
		}

		return false;
	}
	// �� �Լ��� false �̸� Ű�� ���� ������ �ǹ�
	const bool IsKeyPressing(const INPUT_KEY_INFO* pKey) const
	{
		if (nullptr == pKey)
			return false;

		for (auto iter = pKey->vKeys.begin(); iter != pKey->vKeys.end(); ++iter)
		{
			if (EINPUT_KEY_STATE::PRESSING <= get<1>(*iter))
				return true;
		}

		return false;
	}
	// Release ���� Ȯ�ο�
	const bool IsKeyReleased(const INPUT_KEY_INFO* pKey) const
	{
		if (nullptr == pKey)
			return false;

		for (auto iter = pKey->vKeys.begin(); iter != pKey->vKeys.end(); ++iter)
		{
			if (EINPUT_KEY_STATE::RELEASED == get<1>(*iter))
				return true;
		}

		return false;
	}
};


bool CKeyMgr::Key_Pressing(const int& iKey)
{
	if (Engine::Get_DIKeyState(iKey))
		return true;

	return false;
}

bool CKeyMgr::Key_Down(const int& iKey)
{
	// �������� ���� ���� ���� ���� ������ ���
	if (!m_bKeyState[iKey] && (Engine::Get_DIKeyState(iKey)))
		return true;

	return false;
}

bool CKeyMgr::Key_Up(const int& iKey)
{
	// �������� ���� ���� �ְ� ���� ������ �ʾ��� ���
	if (m_bKeyState[iKey] && !(Engine::Get_DIKeyState(iKey)))
	{
		return true;
	}

	return false;
}

bool CKeyMgr::Mouse_Pressing(const MOUSEKEYSTATE& iMouse)
{
	if (iMouse >= 0L && iMouse <= DIM_MB)
	{
		if (Engine::Get_DIMouseState((Engine::MOUSEKEYSTATE)iMouse))
			return true;
	}
	else if (iMouse >= DIM_MB && iMouse < DIM_END)
	{
		switch ((Engine::MOUSEKEYSTATE)iMouse)
		{
		case Engine::DIM_MWU:
		{
			if ((Engine::Get_DIMouseMove(DIMS_Z) > 0L))
				return true;
			break;
		}
		case Engine::DIM_MWD:
		{
			if ((Engine::Get_DIMouseMove(DIMS_Z) < 0L))
				return true;
			break;
		}
		}
	}

	return false;
}

bool CKeyMgr::Mouse_Down(const MOUSEKEYSTATE& iMouse)
{
	if (iMouse >= 0L && iMouse <= DIM_MB)
	{
		if (!m_bMouseState[iMouse] && Engine::Get_DIMouseState((Engine::MOUSEKEYSTATE)iMouse))
			return true;
	}
	else if (iMouse >= DIM_MB && iMouse < DIM_END)
	{
		switch ((Engine::MOUSEKEYSTATE)iMouse)
		{
		case Engine::DIM_MWU:
		{
			if (!m_bMouseState[iMouse] && (Engine::Get_DIMouseMove(DIMS_Z) > 0L))
				return true;
			break;
		}
		case Engine::DIM_MWD:
		{
			if (!m_bMouseState[iMouse] && (Engine::Get_DIMouseMove(DIMS_Z) < 0L))
				return true;
			break;
		}
		}
	}

	return false;
}

bool CKeyMgr::Mouse_Up(const MOUSEKEYSTATE& iMouse)
{
	if (iMouse >= 0L && iMouse <= DIM_MB)
	{
		if (m_bMouseState[iMouse] && !Engine::Get_DIMouseState((Engine::MOUSEKEYSTATE)iMouse))
			return true;
	}
	else if (iMouse >= DIM_MB && iMouse < DIM_END)
	{
		switch ((Engine::MOUSEKEYSTATE)iMouse)
		{
		case Engine::DIM_MWU:
		{
			if (m_bMouseState[iMouse] && !(Engine::Get_DIMouseMove(DIMS_Z) > 0L))
				return true;
			break;
		}
		case Engine::DIM_MWD:
		{
			if (m_bMouseState[iMouse] && !(Engine::Get_DIMouseMove(DIMS_Z) < 0L))
				return true;
			break;
		}
		}
	}

	return false;
}


END