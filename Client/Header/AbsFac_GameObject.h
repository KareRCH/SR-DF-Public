#pragma once

#include "Engine_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class FSerialize_GameObject;

END

/// <summary>
/// �Ľ̵� ������ ���� �߻� ���丮
/// </summary>
class CAbsFac_GameObject
{
private:
	CAbsFac_GameObject() = delete;
	CAbsFac_GameObject(const CAbsFac_GameObject& rhs) = delete;
	~CAbsFac_GameObject() = delete;


public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObject);
};

