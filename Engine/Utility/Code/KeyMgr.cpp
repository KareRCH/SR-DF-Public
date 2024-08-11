#include "KeyMgr.h"

#include "Export_System.h"

IMPLEMENT_SINGLETON(CKeyMgr)

CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
}

CKeyMgr::~CKeyMgr()
{
}

void CKeyMgr::Free()
{
	if (m_mapInputKeys.empty())
		return;

	/*for_each(m_mapInputKeys.begin(), m_mapInputKeys.end(), [](pair<const _tchar*, vector<INPUT_KEY_INFO*>>& vecInputKeyInfo) {
		for_each(vecInputKeyInfo.second.begin(), vecInputKeyInfo.second.end(), ()[INPUT_KEY_INFO*& pInputKey] {
			Safe_Delete(pInputKey);
			})
		});*/
}

HRESULT CKeyMgr::Ready_Key()
{
	Add_InputKey(new INPUT_KEY_INFO(L"Right"));
	//m_vInputKeys.back()->Add_Keys(VK_RIGHT, 'D');
	Add_InputKey(new INPUT_KEY_INFO(L"Left"));
	//m_vInputKeys.back()->Add_Keys(VK_LEFT, 'A');
	Add_InputKey(new INPUT_KEY_INFO(L"Up"));
	//m_vInputKeys.back()->Add_Key(VK_UP);
	Add_InputKey(new INPUT_KEY_INFO(L"Down"));
	//m_vInputKeys.back()->Add_Key(VK_DOWN);

	Add_InputKey(new INPUT_KEY_INFO(L"UI_Accept"));
	//m_vInputKeys.back()->Add_Key(VK_ACCEPT);
	Add_InputKey(new INPUT_KEY_INFO(L"UI_Cancel"));
	//m_vInputKeys.back()->Add_Key(VK_ESCAPE);
	Add_InputKey(new INPUT_KEY_INFO(L"UI_Menu"));
	//m_vInputKeys.back()->Add_Key(VK_ACCEPT);

	return S_OK;
}

void CKeyMgr::Update_Key()
{
	Engine::Update_InputDev();		// 다이렉트 인풋 업데이트를 키 매니저에서 해주어 일관성을 유지한다.
}

void CKeyMgr::LateUpdate_Key()
{
	//for (int i = 0; i < m_vInputKeys.size(); ++i)
	//{
	//	if (nullptr == m_vInputKeys[i])
	//		continue;

	//	for (int j = 0; j < m_vInputKeys[i]->vKeys.size(); ++j)
	//	{
	//		// 입력을 받았을 때
	//		if (GetAsyncKeyState(get<0>(m_vInputKeys[i]->vKeys[j])) & 0x8000)
	//		{
	//			if (EINPUT_KEY_STATE::NUETRAL >= get<1>(m_vInputKeys[i]->vKeys[j]))
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::PRESSED;
	//			else
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::PRESSING;
	//		}
	//		// 입력이 없을 때
	//		else
	//		{
	//			if (EINPUT_KEY_STATE::PRESSING <= get<1>(m_vInputKeys[i]->vKeys[j]))
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::RELEASED;
	//			else
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::NUETRAL;
	//		}
	//	}
	//}

	// m_bKeyState에 대해 키 상태를 업데이트 합니다.
	for (int i = 0; i < MAX_DINPUT_KEY; ++i)
	{
		if (m_bKeyState[i] && !(Engine::Get_DIKeyState(i)))
			m_bKeyState[i] = !m_bKeyState[i];
		else if (!m_bKeyState[i] && (Engine::Get_DIKeyState(i)))
			m_bKeyState[i] = !m_bKeyState[i];
	}

	for (int i = 0; i < MAX_DINPUT_MOUSE; ++i)
	{
		if (i <= 2)
		{
			if (m_bMouseState[i] && !(Engine::Get_DIMouseState((Engine::MOUSEKEYSTATE)i)))
				m_bMouseState[i] = !m_bMouseState[i];
			else if (!m_bMouseState[i] && (Engine::Get_DIMouseState((Engine::MOUSEKEYSTATE)i)))
				m_bMouseState[i] = !m_bMouseState[i];
		}
		else
		{
			switch((Engine::MOUSEKEYSTATE)i)
			{
			case Engine::DIM_MWU:
			{
				if (m_bMouseState[i] && !(Engine::Get_DIMouseMove(DIMS_Z) > 0L))
					m_bMouseState[i] = !m_bMouseState[i];
				else if (!m_bMouseState[i] && (Engine::Get_DIMouseMove(DIMS_Z) > 0L))
					m_bMouseState[i] = !m_bMouseState[i];
				break;
			}
			case Engine::DIM_MWD:
			{
				if (m_bMouseState[i] && !(Engine::Get_DIMouseMove(DIMS_Z) < 0L))
					m_bMouseState[i] = !m_bMouseState[i];
				else if (!m_bMouseState[i] && (Engine::Get_DIMouseMove(DIMS_Z) < 0L))
					m_bMouseState[i] = !m_bMouseState[i];
				break;
			}
			}
		}
	}
}

void CKeyMgr::Add_InputKey(INPUT_KEY_INFO* pInputKey)
{
}

void CKeyMgr::Remove_InputKey(INPUT_KEY_INFO* pInputKey)
{
}
