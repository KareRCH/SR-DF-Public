#pragma once

#include "GameObject.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CRectObject : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CRectObject)

private:
	explicit CRectObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRectObject(const CRectObject& rhs);
	virtual ~CRectObject();

public:
	static CRectObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey);
	static CRectObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

private: // 함수 
	virtual void			Free();											// Release

public:
	virtual HRESULT			Ready_GameObject() override;
	virtual HRESULT			Ready_GameObject(const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey);
	virtual HRESULT			Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int			Update_GameObject(const _float& fTimeDelta) override;
	virtual void			LateUpdate_GameObject() override;
	virtual void			Render_GameObject() override;

private:
	HRESULT					Add_Component();								// 컴포넌트 추가

public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pRectBufferComp, RectBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CRcBufferComp* m_pRectBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;

public:
	GETSET_EX2(_bool, m_bIsSelected, IsSelected, GET_REF, SET_C_REF)

private:
	_bool m_bIsSelected = false;
};

