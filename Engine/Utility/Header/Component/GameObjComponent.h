#pragma once

#include "Component.h"

BEGIN(Engine)

/// <summary>
/// 컴포넌트의 파생으로 게임오브젝트에 추가되어 쓰이는 컴포넌트의 형태이다.
/// 현재는 정착되기 전이라 직접적으로 게임오브젝트에서 쓰이지 않고 CComponent를 쓴다.
/// 추후 추가적인 기능을 제공하기 위해 다형성을 위한 구조를 만들어보려 임시로 나누었다.
/// </summary>
class ENGINE_DLL CGameObjComponent abstract : public CComponent
{
	DERIVED_CLASS(CComponent, CGameObjComponent)

protected:
	explicit CGameObjComponent();
	explicit CGameObjComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObjComponent(const CGameObjComponent& rhs);
	virtual ~CGameObjComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}
};

END