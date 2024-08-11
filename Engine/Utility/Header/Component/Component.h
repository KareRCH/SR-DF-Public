#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;

/// <summary>
/// 모든 컴포넌트 클래스의 베이스 클래스이다.
/// </summary>
class _declspec(dllexport) CComponent : public CBase
{
	DERIVED_CLASS(CBase, CComponent)
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

protected:
	_bool					m_bClone;		// 인스턴스 복사본 여부

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}

protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;	// 렌더 장치
	
public:
	GETSET_EX3(CGameObject*, m_pOwner, Owner, SET, GET, GET_C_C)

protected:
	CGameObject*			m_pOwner;		// 오너 오브젝트
	
};

END