#pragma once

#include "AceObjectFactory.h"
#include "Solid.h"

#include "Serialize_BaseClass.h"

class CAceBuilding : public CSolid
{
	DERIVED_CLASS(CSolid, CAceBuilding)

	enum class BUILDING_NAME { 
		A, // 2층집
		B, // 파란아파트 
		C, // 갈색문 3층
		D, // 1층에 상점있는 오피스텔 
		E, // 붉은벽돌 3층 빌라 
		F, // 학교
		BUILDING_END};
	
private:
	explicit CAceBuilding(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceBuilding(const CAceBuilding& rhs);
	virtual ~CAceBuilding();


public:
	static CAceBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev,
			const _tchar* pObjTag,
			const _float _fx,
			const _float _fy,
			const _float _fz,
			CAceObjectFactory::OBJECT_DIRECTION pDirection = CAceObjectFactory::OBJECT_DIRECTION::SOUTH);

	static CAceBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	virtual void OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionExited(CGameObject* pDst) override;

private: // 함수 
	virtual void		Free();											// Release
	virtual HRESULT		Add_Component();								// 컴포넌트 추가 
	
};
