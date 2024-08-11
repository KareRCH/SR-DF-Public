#include "PHY_Box.h"

FPHY_Box::FPHY_Box()
{
}

FPHY_Box::FPHY_Box(const FPHY_Shape& rhs)
	: Base(rhs)
{
	Free();
}

FPHY_Box::~FPHY_Box()
{
}

FPHY_Box* FPHY_Box::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Ready_Shape()))
	{
		Safe_Release(pInstance);

		MSG_BOX("FPHY_Box Create Failed");
		return nullptr;
	}

	return pInstance;
}

FPHY_Box* FPHY_Box::Clone()
{
	return new ThisClass(*this);
}

void FPHY_Box::Free()
{
	SUPER::Free();
}

HRESULT FPHY_Box::Ready_Shape()
{
	SUPER::Ready_Shape();
	eType = EPHY_SHAPE::BOX;

	return S_OK;
}
