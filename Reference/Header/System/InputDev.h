#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CInputDev : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CInputDev)

private:
	explicit CInputDev(void);
	virtual ~CInputDev(void);

public:
	virtual void	Free(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return (m_byKeyState[byKeyID] & 0x80);
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return (m_tMouseState.rgbButtons[eMouse] & 0x80);
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return (*(((_long*)&m_tMouseState) + eMouseState));
	}

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// Ű���忡 �ִ� ��� Ű���� �����ϱ� ���� ����
	DIMOUSESTATE			m_tMouseState;


};

END