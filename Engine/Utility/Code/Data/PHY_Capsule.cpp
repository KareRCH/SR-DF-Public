#include "PHY_Capsule.h"

FPHY_Capsule::FPHY_Capsule()
{
}

FPHY_Capsule::FPHY_Capsule(const FPHY_Shape& rhs)
	: Base(rhs)
{
}

FPHY_Capsule::~FPHY_Capsule()
{
	Free();
}

FPHY_Capsule* FPHY_Capsule::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Ready_Shape()))
	{
		Safe_Release(pInstance);

		MSG_BOX("FPHY_Capsule Create Failed");
		return nullptr;
	}

	return pInstance;
}

FPHY_Capsule* FPHY_Capsule::Clone()
{
	return new ThisClass(*this);
}

void FPHY_Capsule::Free()
{
	SUPER::Free();
}

HRESULT FPHY_Capsule::Ready_Shape()
{
	SUPER::Ready_Shape();
	eType = EPHY_SHAPE::CAPSULE;

	m_fRadius = 3;
	m_fLengthLine = 3;
		
	return S_OK;
}
