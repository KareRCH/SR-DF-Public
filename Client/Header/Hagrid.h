#pragma once
#include "AceUnit.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CHagrid : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CHagrid)

protected:
	explicit CHagrid(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHagrid(const CHagrid& rhs);
	virtual ~CHagrid();
	virtual void	Free();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	static CHagrid* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, _vec3 _size);
	static CHagrid* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial);

private:
	HRESULT				Add_Component();
	void				Height_On_Terrain();

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;

	_vec3						m_vPos = { 0.f, 0.f, 0.f };

private:
	HRESULT						Billboard(const _float& fTimeDelta);
	//HRESULT					Get_PlayerPos();
	FRAME						m_tFrame;
	_bool		m_bOnce = false;
};