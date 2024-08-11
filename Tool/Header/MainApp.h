#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "imgui.h"

class Engine::CGraphicDev;
class Engine::CManagement;
struct ImGuiIO;

class CMainApp : public CBase
{
	DERIVED_CLASS(CBase, CMainApp)
private:		// 생성자, 소멸자
	explicit CMainApp();
	virtual ~CMainApp();

public:			// 일반적인 함수, 변수
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp();
	void		Render_MainApp();

public:		// 생성 함수
	static		CMainApp* Create();

private:	// 소멸 함수
	virtual		void			Free();

protected:		// 상속관련 함수, 변수
private:
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	void		CleanupDeviceD3D();


private:		// 접근을 최소화하는 함수, 변수
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

private:		// IMGUI 관련 변수
	ImGuiIO* m_io;
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	LPDIRECT3DSURFACE9 m_pOriginalRenderTarget;

public:
	// 디바이스의 매개변수에 창 사이즈를 바꿔 초기화를 다시해줍니다.
	void ResetDevice(_uint dwResizeWidth = 0, _uint dwResizeHeight = 0);

private: // 소영 새로운창 create용
	bool show_map_window = true;

public:
	GETSET_EX2(_uint, m_dwResizeWidth, ResizedWidth, SET_REF, GET_REF)
	GETSET_EX2(_uint, m_dwResizeHeight, ResizedHeight, SET_REF, GET_REF)

private:
	_uint m_dwResizeWidth = 0U;
	_uint m_dwResizeHeight = 0U;


private:
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	_bool				m_bRenderImgui = true;
};
