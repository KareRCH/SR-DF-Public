#pragma once

#include "PHY_Shape.h"

BEGIN(Engine)

class ENGINE_DLL FPHY_Capsule : public FPHY_Shape
{
	DERIVED_CLASS(FPHY_Shape, FPHY_Capsule)
private:
	explicit FPHY_Capsule();
	explicit FPHY_Capsule(const FPHY_Shape& rhs);
	virtual ~FPHY_Capsule();

protected:
	//캡슐 준비물 - 리치거리, 반지름
	float m_fRadius;
	float m_fLengthLine;

public:
	static FPHY_Capsule* Create();
	virtual FPHY_Capsule* Clone();

protected:
	virtual void Free() override;
	virtual HRESULT Ready_Shape();
};

END