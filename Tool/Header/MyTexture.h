#pragma once

#include "GameObject.h"
#include "MyTexture.h"
#include "ImguiAnimationTool.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END



class CMyTexture : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CMyTexture)

private:
	explicit CMyTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMyTexture(const CMyTexture& rhs);
	virtual ~CMyTexture();

public:
	static CMyTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	HRESULT				Add_Component();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTextureComponent* m_pBackTextureComp = nullptr;

	_matrix		m_ViewMatrix, m_ProjMatrix;
private:
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

	// 애니메이션 프레임
	_float m_fAniamtionFrame = 0.f;
	_float m_fAnimationSpeed = 7.f;
	_float m_fMaxFrame = 0.f;

	// 애니메이션 툴
	CImguiAnimationTool* m_pAnimationTool; //오류

	// 애니메이션 값
	std::vector<KEYFRAME>* m_vecAnimationInfo; //오류


	_float fFrameTimeDelta, fCurFrameTimeDelta;	//dt
	_float fSizeX_Delta, fSizeY_Delta;
	_float fRotX_Delta, fRotY_Delta;
	_float fPosX_Delta, fPosY_Delta;

	_bool bFrameOn = false;
};

