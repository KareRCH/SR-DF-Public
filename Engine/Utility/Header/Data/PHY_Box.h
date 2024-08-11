#pragma once

#include "PHY_Shape.h"

BEGIN(Engine)

class ENGINE_DLL FPHY_Box : public FPHY_Shape
{
	DERIVED_CLASS(FPHY_Shape, FPHY_Box)
private:
	explicit FPHY_Box();
	explicit FPHY_Box(const FPHY_Shape& rhs);
	virtual ~FPHY_Box();

protected:
	// 박스의 준비물 - 한변의 길이 


protected:
	virtual HRESULT Ready_Shape();
	virtual void Free() override;

public:
	static FPHY_Box* Create();
	virtual FPHY_Box* Clone();

};

END