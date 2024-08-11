#pragma once

#include "PHY_Shape.h"

BEGIN(Engine)

class ENGINE_DLL FPHY_Sphere : public FPHY_Shape
{
	DERIVED_CLASS(FPHY_Shape, FPHY_Sphere)
private:
	explicit FPHY_Sphere();
	explicit FPHY_Sphere(const FPHY_Shape& rhs);
	virtual ~FPHY_Sphere();

protected:
	_float m_fRadius ; // 구는 반지름 필요 

public:
	static FPHY_Sphere* Create();
	virtual FPHY_Sphere* Clone();

protected:
	virtual void Free() override;
	virtual HRESULT Ready_Shape();
};

END