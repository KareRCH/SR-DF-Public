#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT			Add_Component();

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CCubeBufferComp* m_pCubeBufferComp = nullptr;

public:
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fScale);

private:
	virtual void Free();


};