#pragma once
#include "AceGameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"
BEGIN(Engine)

class CTransformComponent;

END

class CBossLight : public CAceGameObject
{
	DERIVED_CLASS(CAceGameObject, CBossLight)

private:
	explicit CBossLight(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBossLight(const CBossLight& rhs);
	virtual ~CBossLight();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CBossLight*  Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner);

public:
	virtual void OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionExited(CGameObject* pDst) override;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pBossTransformcomp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

	// Get, Set 함수 만들기 ---------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pBossTransformcomp, BossTransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(_bool, m_bLightOn, m_bLightOn, GET, SET)


		//- 블랙보드
public:
	void		Update_BlackBoard();

private:
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster;

private:
	HRESULT Create_Light(); // 점광원 만들기 

	D3DLIGHT9 AceBossLight;		// 점광원 
	_bool m_bLightOn; // 연동해서 가져올 보스 라이터 on상태 

};