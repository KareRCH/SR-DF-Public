#include "stdafx.h"
#include "BlackBoard_Camera.h"

#include "DynamicCamera.h"

CBlackBoard_Camera::CBlackBoard_Camera()
    : Base()
{
}

CBlackBoard_Camera::CBlackBoard_Camera(const CBlackBoard_Camera& rhs)
    : Base(rhs)
{
}

CBlackBoard_Camera::~CBlackBoard_Camera()
{
    Free();
}

CBlackBoard_Camera* CBlackBoard_Camera::Create()
{
    ThisClass* pInstance = new ThisClass();

    if (FAILED(pInstance->Ready_BlackBoard()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BlackBoard_Camera Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CBlackBoard_Camera::Free()
{

}

HRESULT CBlackBoard_Camera::Ready_BlackBoard(const FBlackBoard_CameraInit& tInit)
{
    m_pCamera = tInit.pCamera;

    return S_OK;
}
