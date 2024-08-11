#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

/// <summary>
/// Dynamic ���� �浹ü�� Ÿ���� �����ϱ� ���� �뵵�� ����ü
/// </summary>
enum class EPHY_SHAPE
{
	NONE,
	SPHERE,
	BOX,
	CAPSULE
};

/// <summary>
/// �浹ü ���� ���� Ŭ����
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
	_uint			uiID = 0U;						// �������� �� �ĺ� ���̵�
	EPHY_SHAPE		eType = EPHY_SHAPE::NONE;		// �浹ü Ÿ��
};

END