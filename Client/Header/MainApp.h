#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;

END

class CMainApp : public CBase
{
	DERIVED_CLASS(CBase, CMainApp)
private:		// 생성자, 소멸자
	explicit CMainApp();
	virtual ~CMainApp();

public:	// 생성 함수
	static		CMainApp* Create();
	
private: // 소멸 함수
	virtual		void			Free();

public:			// 일반적인 함수, 변수
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp();
	void		Render_MainApp();

protected:		// 상속관련 함수, 변수
private:
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);

private:		// 접근을 최소화하는 함수, 변수
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3DDEVICE9			m_pGraphicDev;


private:
	void		Render_FrameRate();

};

