#pragma once

#include "Scene.h"

#include "Player.h"
#include "Brown.h"
#include "Gray.h"
#include "AceBoss.h"
#include "AceObjectFactory.h"
#include "PlayerLighter.h"
#include "PlayerGunLighter.h"
#include "BossLight.h"

#include "Terrain.h"
#include "SnowParticle.h"
#include "FireWork.h"
#include "UI_CrossHair.h"
#include "UI_PlayerFace.h"
//test
#include "UI_MonsterHP.h"
#include "Hagrid.h"

class CStage : public Engine::CScene
{
	DERIVED_CLASS(CScene, CStage)

private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	static CStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

	virtual HRESULT		ReadyLate_Scene();

private:
	HRESULT				Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT				Ready_Layer_UI(const _tchar* pLayerTag);

protected:
	virtual HRESULT		Ready_Layer_Completed();


};

