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
private:		// ������, �Ҹ���
	explicit CMainApp();
	virtual ~CMainApp();

public:	// ���� �Լ�
	static		CMainApp* Create();
	
private: // �Ҹ� �Լ�
	virtual		void			Free();

public:			// �Ϲ����� �Լ�, ����
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp();
	void		Render_MainApp();

protected:		// ��Ӱ��� �Լ�, ����
private:
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);

private:		// ������ �ּ�ȭ�ϴ� �Լ�, ����
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3DDEVICE9			m_pGraphicDev;


private:
	void		Render_FrameRate();

};

