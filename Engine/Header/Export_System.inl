#include "Export_System.h"
#pragma region �׷��� ����̽�
HRESULT			Get_DeviceAvailable()
{
	return CGraphicDev::GetInstance()->Get_DeviceReadyState();
}
LPDIRECT3D9		Get_GraphicSDK()
{
	return CGraphicDev::GetInstance()->Get_GraphicSDK();
}
LPDIRECT3DDEVICE9		Get_GraphicDev()
{
	return CGraphicDev::GetInstance()->Get_GraphicDev();
}
D3DPRESENT_PARAMETERS*	Get_D3DPP()
{
	return CGraphicDev::GetInstance()->Get_D3DPP();
}
HRESULT		Ready_GraphicDev(CGraphicDev** ppGraphicClass, HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY)
{
	return CGraphicDev::GetInstance()->Ready_GraphicDev(ppGraphicClass, hWnd, eMode, iSizeX, iSizeY);
}
HRESULT		Render_Begin(D3DXCOLOR Color)
{
	return CGraphicDev::GetInstance()->Render_Begin(Color);
}
HRESULT		Render_End()
{
	return CGraphicDev::GetInstance()->Render_End();
}
#pragma endregion



#pragma region Ÿ�� �Ŵ���
_float			Get_TimeDelta(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Get_TimeDelta(pTimerTag);
}
void				Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimerMgr::GetInstance()->Set_TimeDelta(pTimerTag);
}
HRESULT			Ready_Timer(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Ready_Timer(pTimerTag);
}
#pragma endregion



#pragma region ������ �Ŵ���
// ������ �Ŵ���
_bool			IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	return CFrameMgr::GetInstance()->IsPermit_Call(pFrameTag, fTimeDelta);
}
HRESULT			Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	return CFrameMgr::GetInstance()->Ready_Frame(pFrameTag, fCallLimit);
}
void			Set_FrameRate(const _tchar* pFrameTag, const _float& fFrameRate)
{
	CFrameMgr::GetInstance()->Set_FrameRate(pFrameTag, fFrameRate);
}
const _float&	Get_FrameRate(const _tchar* pFrameTag)
{
	return CFrameMgr::GetInstance()->Get_FrameRate(pFrameTag);
}

#pragma endregion




#pragma region ��Ʈ
// ��Ʈ �غ��Ű��
HRESULT		Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const _tchar* pFontTag,
	const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	return CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pFontTag, pFontType, iWidth, iHeight, iWeight);
}
// ��Ʈ �������ϱ�
void		Render_Font(const _tchar* pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color)
{
	CFontMgr::GetInstance()->Render_Font(pFontTag, pString, pPos, Color);
}
#pragma endregion




#pragma region �Է� ��ġ
// Ű���� ���� ���¸� ����
_byte	Get_DIKeyState(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Get_DIKeyState(byKeyID);
}
// ���콺 ���� ���¸� ����
_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Get_DIMouseState(eMouse);
}
// ���콺 �����ӿ� ���� ����
_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return CInputDev::GetInstance()->Get_DIMouseMove(eMouseState);
}
// �Է� �ʱ�ȭ
HRESULT	Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	return CInputDev::GetInstance()->Ready_InputDev(hInst, hWnd);
}
// �Է� ������Ʈ
void		Update_InputDev(void)
{
	CInputDev::GetInstance()->Update_InputDev();
}

#pragma endregion






// ���� �Ŵ���
HRESULT	Ready_SoundDev()
{
	return CSoundMgr::GetInstance()->Ready_Sound();
}

void	Play_Sound(const _tchar* pCategoryKey, const _tchar* pSoundKey, CHANNELID eChannel, _float fVolume)
{
	Engine::CSoundMgr::GetInstance()->Play_Sound(const_cast<_tchar*>(pCategoryKey), 
													const_cast<_tchar*>(pSoundKey), 
													eChannel, fVolume);
}

void	Play_BGM(const _tchar* pCategoryKey, const _tchar* pSoundKey, _float fVolume)
{
	Engine::CSoundMgr::GetInstance()->Play_BGM(const_cast<_tchar*>(pCategoryKey),
													const_cast<_tchar*>(pSoundKey), fVolume);
}

inline void Stop_Sound(CHANNELID eID)
{
	Engine::CSoundMgr::GetInstance()->Stop_Sound(eID);
}

inline void StopAll()
{
	Engine::CSoundMgr::GetInstance()->StopAll();
}

void	Release_System()
{
	CFontMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CInputDev::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
	CSoundMgr::GetInstance()->DestroyInstance();
}