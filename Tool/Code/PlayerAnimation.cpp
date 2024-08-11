#include "PlayerAnimation.h"

//CPlayerAnimation::CPlayerAnimation()
//{
//}

CPlayerAnimation::~CPlayerAnimation()
{
}

CPlayerAnimation* CPlayerAnimation::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    //CPlayerAnimation* pInstance = new CPlayerAnimation;
    //if (FAILED(pInstance->Ready_GameObject()))
    {
    //    Safe_Release(pInstance);
    //    return nullptr;
    }

    //return pInstance;
    return 0;
}

HRESULT CPlayerAnimation::Ready_GameObject()
{

	return S_OK;
}

_int CPlayerAnimation::Update_GameObject(const _float& fTimeDelta)
{
	return 0;
}

void CPlayerAnimation::LateUpdate_GameObject()
{
}

void CPlayerAnimation::Render_GameObject()
{
}

void CPlayerAnimation::Free()
{
}
