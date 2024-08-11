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
private:		// ������, �Ҹ���
	explicit CMainApp();
	virtual ~CMainApp();

public:			// �Ϲ����� �Լ�, ����
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp();
	void		Render_MainApp();

public:		// ���� �Լ�
	static		CMainApp* Create();

private:	// �Ҹ� �Լ�
	virtual		void			Free();

protected:		// ��Ӱ��� �Լ�, ����
private:
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	void		CleanupDeviceD3D();


private:		// ������ �ּ�ȭ�ϴ� �Լ�, ����
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

private:		// IMGUI ���� ����
	ImGuiIO* m_io;
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	LPDIRECT3DSURFACE9 m_pOriginalRenderTarget;

public:
	// ����̽��� �Ű������� â ����� �ٲ� �ʱ�ȭ�� �ٽ����ݴϴ�.
	void ResetDevice(_uint dwResizeWidth = 0, _uint dwResizeHeight = 0);

private: // �ҿ� ���ο�â create��
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
