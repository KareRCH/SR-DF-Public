#include "stdafx.h"
#include "AbsFac_GameObject.h"

#include "Player.h"
#include "Brown.h"
#include "Gray.h"
#include "AceBoss.h"
#include "AceFood.h"
#include "AceWeapon.h"
#include "AceThrow.h"
#include "AceInteraction.h"
#include "AceBuilding.h"
#include "Trigger_ToStageHall.h"
#include "Trigger_ToJumpMap.h"
#include "Trigger_ToBossMap.h"
#include "Trigger_Box.h"
#include "JumpBgm.h"
#include "JumpBat.h"
#include "JumpMonster.h"
#include "JumpMainBgm.h"
#include "GateWay1.h"
#include "Hagrid.h"

#include "Serialize_BaseClass.h"

CGameObject* CAbsFac_GameObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObject)
{
	//EGO_CLASS eClassType = tObject.eID;

	//switch (eClassType)
	//{
	//case Engine::ECLASS_NONE:
	//	break;
	//case Engine::ECLASS_PLAYER:
	//	return static_cast<CGameObject*>(CPlayer::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_BROWN:
	//	return static_cast<CGameObject*>(CBrown::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_GREY:
	//	return static_cast<CGameObject*>(CGray::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_BOSS:
	//	//return static_cast<CGameObject*>(CAceBoss::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_FOOD:
	//	return static_cast<CGameObject*>(CAceFood::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_WEAPON:
	//	return static_cast<CGameObject*>(CAceWeapon::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_THROW:
	//	return static_cast<CGameObject*>(CAceThrow::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_INTERACTION:
	//	return static_cast<CGameObject*>(CAceInteraction::Create(pGraphicDev, tObject));
	//	break;
	//case Engine::ECLASS_BUILDING:
	//	return static_cast<CGameObject*>(CAceBuilding::Create(pGraphicDev, tObject));
	//	break;
	//default:
	//	break;
	//}

	const string& strType = tObject.strClassName;

	if (strType == "AceBuilding")
	{
		return static_cast<CGameObject*>(CAceBuilding::Create(pGraphicDev, tObject));
	}
	else if (strType == "AceFood")
	{
		return static_cast<CGameObject*>(CAceFood::Create(pGraphicDev, tObject));
	}
	else if (strType == "AceThrow")
	{
		return static_cast<CGameObject*>(CAceThrow::Create(pGraphicDev, tObject));
	}
	else if (strType == "AceWeapon")
	{
		return static_cast<CGameObject*>(CAceWeapon::Create(pGraphicDev, tObject));
	}
	else if (strType == "AceInteraction")
	{
		return static_cast<CGameObject*>(CAceInteraction::Create(pGraphicDev, tObject));
	}
	else if (strType == "Brown")
	{
		return static_cast<CGameObject*>(CBrown::Create(pGraphicDev, tObject));
	}
	else if (strType == "Gray")
	{
		return static_cast<CGameObject*>(CGray::Create(pGraphicDev, tObject));
	}
	else if (strType == "AceBoss")
	{
		return static_cast<CGameObject*>(CAceBoss::Create(pGraphicDev, tObject));
	}
	else if (strType == "Player")
	{
		return static_cast<CGameObject*>(CPlayer::Create(pGraphicDev, tObject));
	}
	else if (strType == "Trigger_ToStageHall")
	{
		return static_cast<CGameObject*>(CTrigger_ToStageHall::Create(pGraphicDev, tObject));
	}
	else if (strType == "Trigger_ToBossMap")
	{
		return static_cast<CGameObject*>(CTrigger_ToBossMap::Create(pGraphicDev, tObject));
	}
	else if (strType == "Trigger_ToJumpMap")
	{
		return static_cast<CGameObject*>(CTrigger_ToJumpMap::Create(pGraphicDev, tObject));
	}
	else if (strType == "Trigger_Box")
	{
		return static_cast<CGameObject*>(CTrigger_Box::Create(pGraphicDev, tObject));
	}
	else if (strType == "JumpBgm")
	{
		return static_cast<CGameObject*>(CJumpBgm::Create(pGraphicDev, tObject));
	}
	else if (strType == "JumpBat")
	{
		return static_cast<CGameObject*>(CJumpBat::Create(pGraphicDev, tObject));
	}
	else if (strType == "JumpMonster")
	{
		return static_cast<CGameObject*>(CJumpMonster::Create(pGraphicDev, tObject));
	}
	else if (strType == "JumpMainBgm")
	{
		return static_cast<CGameObject*>(CJumpMainBgm::Create(pGraphicDev, tObject));
	}
	else if (strType == "GateWay1")
	{
		return static_cast<CGameObject*>(CGateWay1::Create(pGraphicDev, tObject));
	}

	else if (strType == "Hagrid")
	{
		return static_cast<CGameObject*>(CHagrid::Create(pGraphicDev, tObject));
	}
	return static_cast<CGameObject*>(nullptr);
}
