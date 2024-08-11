#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera :  public CGameObject
{
	DERIVED_CLASS(CGameObject, CCamera)
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera();

protected:
	virtual void	Free();

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject() {}

public:
	GETSET_EX2(_vec3, m_vEye, Eye, SET, GET)
	GETSET_EX2(_vec3, m_vAt, At, GET, SET)
	GETSET_EX2(_vec3, m_vUp, Up, GET, SET)

	GETSET_EX2(_bool, m_bOne, One, GET, SET)
	GETSET_EX2(_bool, m_bThree, Three, GET, SET)
	GETSET_EX2(_bool, m_Change, Change, GET, SET)
	GETSET_EX2(_bool, m_Change2, Change2, GET, SET)

protected:
	_vec3			m_vEye, m_vAt, m_vUp;
	_float			m_fFov, m_fAspect, m_fNear, m_fFar;
	_matrix			m_matView, m_matProj;

	_float			m_fWidth, m_fHeight;

	// + ¼ºÈñ Ãß°¡
	//1ÀÎÄª ½ºÀ§Ä¡
	_bool		m_Change = false;
	_bool		m_bOne = false;

	//3ÀÎÄª ½ºÀ§Ä¡
	_bool		m_Change2 = false;
	_bool		m_bThree = false;

};

END