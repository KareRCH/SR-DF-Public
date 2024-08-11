#pragma once
#include "AceGameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"

BEGIN(Engine)

class CTransformComponent;

END

class CPlayerGunLighter : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CPlayerGunLighter)

private:
	explicit CPlayerGunLighter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerGunLighter(const CPlayerGunLighter& rhs);
	virtual ~CPlayerGunLighter();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CPlayerGunLighter* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

	// Get, Set �Լ� ����� ---------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
		GETSET_EX2(CTransformComponent*, m_pPlayerTransformcomp, PlayerTransformComponent, GET, SET)
		GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
		GETSET_EX2(_bool, m_bLightOn, m_bLightOn, GET, SET)

private:
	HRESULT Create_Light(); // ������ ����� 

	D3DLIGHT9 AceLight;		// ������ 
	_bool m_bLightOn; // ������ �����ؼ� ������ �÷��̾� ������ on���� 

	// ������
public:
	void		Update_BlackBoard();

private:
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;
};