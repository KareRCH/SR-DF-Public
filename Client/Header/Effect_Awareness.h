#pragma once
#include "AceEffect.h"

class CEffect_Awareness : public CAceEffect
{
	DERIVED_CLASS(CAceEffect, CEffect_Awareness)

	PUBLIC enum class TYPE {BROWN, GRAY, BOSS, TYPE_END};

private:
	explicit CEffect_Awareness(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_Awareness(const CEffect_Awareness& rhs);
	virtual ~CEffect_Awareness();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CEffect_Awareness* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CEffect_Awareness::TYPE pType, CGameObject* pOwner);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

private:
	CEffect_Awareness::TYPE m_eType;

public:
	_bool		m_bTrigger = FALSE; // 변수 다 채웠을때 
	_bool		m_bMissTarget = FALSE; // 플레이어 포착 못했을 때 

	_float		m_fAwareness = 0.f; // 현재값
	_float		m_fMaxAwareness = 0.f; // max값
	_float		m_fAwarenessPrev = 0.f; // 이전값

	void Owner_Get_Awareness(CGameObject* pDst);


};
	

