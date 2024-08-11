#pragma once

#include "AceObjectFactory.h"
#include "AceUnit.h"
#include "Engine_Enum.h"
#include "Serialize_BaseClass.h"
#include "Player.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CAceWeapon : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceWeapon)

private:
	explicit CAceWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceWeapon(const CAceWeapon& rhs);
	virtual ~CAceWeapon();

public:
	static CAceWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
	static CAceWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial );

private:
	virtual void		Free();

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

public:
	CPlayer::OBJECT_NAME Get_WeaponName() { return m_pCurName; }
	GETSET_EX2(_bool, m_bWeaponGearup, Weapon_GearUp, GET, SET)

private: // 함수 
	HRESULT				Add_Component();
	HRESULT				BillBoard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	void				Height_On_Terrain();
	void				WeaponName(const _tchar* pObjTag);
	void				WeaponName(wstring pObjTag);

	void				Change_Texture(CPlayer::OBJECT_NAME eReceiveName); //모든 무기에 내구도에 따른 이미지 존재 


private: // 변수 
	CPlayer::OBJECT_NAME			m_pCurName;
	OBJECT_CLASS		m_eFactoryClass;
	_float m_fHp = 2.f;
	_float m_fBrokenHp = 0.f;
	_float m_fCrackedHp = 1.f;
	_float m_fMaxHP = 2.f;
	_bool m_bWeaponGearup = false;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	
};

