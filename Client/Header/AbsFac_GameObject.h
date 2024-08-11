#pragma once

#include "Engine_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class FSerialize_GameObject;

END

/// <summary>
/// 파싱된 데이터 전용 추상 팩토리
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

