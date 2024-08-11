#pragma once

#include "GraphicDev.h"
#include "TimerMgr.h"
#include "FrameMgr.h"
#include "FontMgr.h"
#include "InputDev.h"
#include "SoundMgr.h"

/*
* DLL에 포함된 모든 외부용 함수들을 헤더로 만든다.
*/

BEGIN(Engine)

// GraphicDev
inline HRESULT					Get_DeviceAvailable();
inline LPDIRECT3D9				Get_GraphicSDK();
inline LPDIRECT3DDEVICE9		Get_GraphicDev();
inline D3DPRESENT_PARAMETERS*	Get_D3DPP();
inline HRESULT					Ready_GraphicDev(CGraphicDev** ppGraphicClass, HWND hWnd, WINMODE eMode, const _uint& iSizeX = WINCX, const _uint& iSizeY = WINCY);
inline HRESULT					Render_Begin(D3DXCOLOR Color);
inline HRESULT					Render_End();

// TimerMgr
inline _float				Get_TimeDelta(const _tchar* pTimerTag);
inline void					Set_TimeDelta(const _tchar* pTimerTag);
inline HRESULT				Ready_Timer(const _tchar* pTimerTag);
inline void					Release_System();

// FrameMgr
inline _bool				IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
inline HRESULT				Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);
inline void					Set_FrameRate(const _tchar* pFrameTag, const _float& fFrameRate);
inline const _float&		Get_FrameRate(const _tchar* pFrameTag);


// FontMgr
inline HRESULT		Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const _tchar* pFontTag,
	const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight);

inline void		Render_Font(const _tchar* pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color);

// InputDev
inline _byte			Get_DIKeyState(_ubyte byKeyID);
inline _byte			Get_DIMouseState(MOUSEKEYSTATE eMouse);
inline _long			Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

inline HRESULT			Ready_InputDev(HINSTANCE hInst, HWND hWnd);
inline void				Update_InputDev(void);



// SoundMgr
inline HRESULT			Ready_SoundDev();
inline void				Play_Sound(const _tchar* pCategoryKey, const _tchar* pSoundKey, CHANNELID eChannel, _float fVolume);
inline void				Play_BGM(const _tchar* pCategoryKey, const _tchar* pSoundKey, _float fVolume);
inline void				Stop_Sound(CHANNELID eID);
inline void				StopAll();

inline void				Release_System();
#include "Export_System.inl"

END