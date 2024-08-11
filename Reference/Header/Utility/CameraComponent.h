#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCameraComponent : public CComponent
{
	DERIVED_CLASS(CComponent, CCameraComponent)

private:
	explicit CCameraComponent();
	explicit CCameraComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCameraComponent(const CCameraComponent& rhs);
	virtual ~CCameraComponent();

public:
	HRESULT			Ready_Camera(HWND hWnd);
	virtual _int	Update_Component(const _float& fTimeDelta);
	void			Input(const _float& fTimeDelta);

public:
	static CCameraComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd);
	virtual CComponent* Clone();

private:
	virtual void Free();

public:
	_matrix* Get_ViewMatrix() { return &m_matView; }
	_matrix* Get_ProjectionMatrix() { return &m_matProjection; }

protected:
	HWND m_hWnd;

	_vec3 m_vPos;
	_vec3 m_vLook;
	_vec3 m_vUp;

	_matrix m_matView;				// 뷰 행렬 저장
	_matrix m_matProjection;		// 투영 행렬 저장

	// 이전 프레임의 방향 벡터
	_vec3 m_vLook_PreFrame;

private:
	
};

END