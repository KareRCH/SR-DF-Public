#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

/// <summary>
/// Dynamic 없이 충돌체의 타입을 구분하기 위한 용도의 열거체
/// </summary>
enum class EPHY_SHAPE
{
	NONE,
	SPHERE,
	BOX,
	CAPSULE
};

/// <summary>
/// 충돌체 정보 저장 클래스
/// </summary>
class ENGINE_DLL FPHY_Shape abstract : public CBase
{
	DERIVED_CLASS(CBase, FPHY_Shape)
protected:
	explicit FPHY_Shape() {}
	explicit FPHY_Shape(const FPHY_Shape& rhs)
		: uiID(rhs.uiID), eType(rhs.eType)
	{}
	virtual ~FPHY_Shape() {}

public:
	//static FPHY_Shape* Create();
	virtual FPHY_Shape* Clone() PURE;
protected:
	virtual void Free() {}

protected:
	virtual HRESULT Ready_Shape();

public:
	static _uint uiID_Count;

public:
	_uint			uiID = 0U;						// 물리세계 내 식별 아이디
	EPHY_SHAPE		eType = EPHY_SHAPE::NONE;		// 충돌체 타입
};

END