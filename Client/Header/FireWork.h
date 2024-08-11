#pragma once
#include "Psystem.h"
#include "Export_System.h"
#include "Export_Utility.h"

class CFireWork : public Engine::CPsystem
{
	DERIVED_CLASS(CPsystem, CFireWork)

private:
	explicit CFireWork(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFireWork();

public:
	HRESULT					Ready_GameObject(_vec3 vOriginPos, _int numParticles);
	_int					Update_GameObject(const _float& fTimeDelta);
	void					LateUpdate_GameObject(void);
	void					Render_GameObject() override;

	static CFireWork* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vOriginPos, _int numParticles);


private:
	void					ResetParticle(Attribute* _attribute) override;
	virtual void			Free();

private:
	_float	    m_fTime = 0.f;
	_float      m_fMoveTime = 20.f;
	_float      m_fDieTime = 20.f;

//=============== 셰이더 연습 ===============
private:
	LPD3DXEFFECT			gpLightingShader = NULL; // 쉐이더 - L4용 
	LPD3DXEFFECT			gpTextureMappingShader = NULL; // 쉐이더 - L3용
	LPD3DXMESH				gpSphere = NULL; // 모델
	LPDIRECT3DTEXTURE9		gpEarthDM = NULL;// 텍스처

	D3DXVECTOR4				gWorldLightPosition = { 500.0f, 500.0f, -500.0f, 1.0f };// 빛의 위치
	D3DXVECTOR4				gWorldCameraPosition = { 0.0f, 0.0f, -200.0f, 1.0f };// 카메라 위치

	LPD3DXEFFECT LoadShader(const _tchar* filename); // 쉐이더 로딩
	LPD3DXMESH LoadModel(const _tchar* filename);// 모델 로딩
	LPDIRECT3DTEXTURE9 LoadTexture(const _tchar* filename);// 텍스처 로딩
	_bool LoadAssets();
};

