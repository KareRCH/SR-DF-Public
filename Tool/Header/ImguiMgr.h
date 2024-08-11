#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;

END

class CImguiWin;
struct ImGuiIO;

class CImguiMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CImguiMgr)

private:
	explicit CImguiMgr();
	virtual ~CImguiMgr();

public:
	virtual void	Free();

public:
	HRESULT Ready_Imgui(CGraphicDev** ppGraphicClass, LPDIRECT3DDEVICE9* ppGraphicDev, _uint iRenderTargetCount = 1U);
	HRESULT Update_Imgui(const _float& fTimeDelta);
	HRESULT Render_Imgui();
	HRESULT Render_AdditionImgui();
	HRESULT Render_LossHandle(HRESULT dwDeviceState);

public:
	// 디바이스의 매개변수에 창 사이즈를 바꿔 초기화를 다시해줍니다.
	void ResetDevice(_uint dwResizeWidth = 0, _uint dwResizeHeight = 0);
	bool LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight);

public:
	GETSET_EX1(Engine::CGraphicDev*, m_pDeviceClass, DeviceClass, GET_REF_C)
	GETSET_EX1(LPDIRECT3DDEVICE9, m_pGraphicDev, GraphicDev, GET)
	GETSET_EX1(vector<LPDIRECT3DTEXTURE9>, m_vecRenderTargetTex, VecRenderTargetTex, GET_C_REF)
	LPDIRECT3DTEXTURE9& Get_VecRenderTargetTex(_uint value) { return m_vecRenderTargetTex[value]; }
	GETSET_EX1(_uint, static_cast<_uint>(m_vecRenderTargetTex.size()), RenderTargetCount, GET_C_REF)
	// ㅋ
private:		// 접근을 최소화하는 함수, 변수
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	vector<LPDIRECT3DTEXTURE9>	m_vecRenderTargetTex;

private:		// IMGUI 관련 변수
	ImGuiIO*	m_pIO;

private:
	bool		m_bShow_DemoWindow =false;
	bool		m_bShow_AnotherWindow =	false;
	bool		m_bShow_MapWindow =	false;								// 소영 새로운창 create용

public:
	void		Sort_ImguiWin();

public:
	using win_map = _unmap<const _tchar*, CImguiWin*>;
	GETSET_EX1(win_map, m_mapImguiWin, MapImguiWin, GET_REF)

private:
	win_map										m_mapImguiWin;			// IMGUI 윈도우 클래스 저장 컨테이너
	vector<pair< const _tchar*, CImguiWin*>>	m_vecSortedImguiWin;	// IMGUI value 기반 정렬 컨테이너

public:
	GETSET_EX2(LPDIRECT3DTEXTURE9, m_pEditorTexture, EditorTexture, SET_PTR, GET_PTR)

private:
	LPDIRECT3DTEXTURE9 m_pEditorTexture = nullptr;

public:
	void HelpMarker(const char* desc) { HelpMarkerEx("(?)", desc); }
	void HelpMarkerEx(const char* marker, const char* desc);
	

public:
	void	Open_Layout(const _tchar* pWin);
	void	Close_Layout(const _tchar* pWin);
};

