#pragma once
#include "AceGameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "BlackBoardPtr.h"
#include "BlackBoard_Player.h"

BEGIN(Engine)

class CTransformComponent;

END

class CPlayerLighter : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CPlayerLighter)

private:
	explicit CPlayerLighter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerLighter(const CPlayerLighter& rhs);
	virtual ~CPlayerLighter();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CPlayerLighter* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	void				Update_InternalData();
	virtual void		Free();

	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;
private:
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

	// Get, Set 함수 만들기 ---------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pPlayerTransformcomp, PlayerTransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(_bool, m_bLightOn, m_bLightOn, GET, SET)

private:
	HRESULT Create_Light(); // 점광원 만들기 

	D3DLIGHT9 AceLight;		// 점광원 
	_bool m_bLightOn; // 블랙보드 연동해서 가져올 플레이어 라이터 on상태 

};