#include "PHY_Shape.h"

_uint FPHY_Shape::uiID_Count = 0U;



//FPHY_Shape* FPHY_Shape::Create()
//{
//	ThisClass* pInstance = new ThisClass();
//
//	if (FAILED(pInstance->Ready_Physics()))
//	{
//		Safe_Release(pInstance);
//
//		MSG_BOX("FPHY_Shape Create Failed");
//		return nullptr;
//	}
//
//	return pInstance;
//}
//
//FPHY_Shape* FPHY_Shape::Clone()
//{
//	return new ThisClass(*this);
//}

HRESULT FPHY_Shape::Ready_Shape()
{
	uiID = uiID_Count++;		// ID 자동 증가

	return S_OK;
}
