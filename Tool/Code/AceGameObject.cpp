#include "AceGameObject.h"

CAceGameObject::CAceGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceGameObject::CAceGameObject(const CAceGameObject& rhs)
	: Base(rhs)
{
}

CAceGameObject::~CAceGameObject()
{
}

HRESULT CAceGameObject::Ready_GameObject()
{
	return S_OK;
}

_int CAceGameObject::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	return 0;
}

void CAceGameObject::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CAceGameObject::Render_GameObject()
{
}

void CAceGameObject::Free()
{
	SUPER::Free();
}
