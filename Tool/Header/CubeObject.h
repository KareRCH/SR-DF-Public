#pragma once

#include "GameObject.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CCubeObject : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CCubeObject)

private:
	explicit CCubeObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeObject(const CCubeObject& rhs);
	virtual ~CCubeObject();

public:
	static CCubeObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vRot, const _vec3& vScale, string strGroupKey, string strTextureKey);
	static CCubeObject*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

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
	GETSET_EX2(CTransformComponent*,	m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCubeBufferComp*,		m_pCubeBufferComp, CubeBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pTextureComp, TextureComponent, GET, SET)

private:
	CTransformComponent*		m_pTransformComp = nullptr;
	CCubeBufferComp*			m_pCubeBufferComp = nullptr;
	CTextureComponent*			m_pTextureComp = nullptr;
	

public:
	GETSET_EX2(_bool, m_bIsSelected, IsSelected, GET_REF, SET_C_REF)

private:
	_bool m_bIsSelected = false;
};
