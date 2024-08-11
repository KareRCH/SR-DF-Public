#include "Light.h"

#include "Export_System.h"
#include "Export_Utility.h"

CLight::CLight(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_iIndex(0)
{
	ZeroMemory(&m_tLightInfo, sizeof(D3DLIGHT9));
	m_pGraphicDev->AddRef();
}


CLight::~CLight()
{
}

CLight* CLight::Create(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Light(pLightInfo, iIndex)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Light Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLight::Free()
{
	m_pGraphicDev->LightEnable(m_iIndex, FALSE);

	Safe_Release(m_pGraphicDev);
}

HRESULT CLight::Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	memcpy(&m_tLightInfo, pLightInfo, sizeof(D3DLIGHT9));
	m_iIndex = iIndex;

	// 조명 설치
	m_pGraphicDev->SetLight(iIndex, pLightInfo);

	// 조명 ON
	m_pGraphicDev->LightEnable(iIndex, TRUE);

	return S_OK;
}


