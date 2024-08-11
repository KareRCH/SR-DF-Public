#pragma once
#include "GameObject.h"
#include "ITeamAgent.h"


/// <summary>
/// ���� ���̽� ���� ���� ������Ʈ
/// �� ������Ʈ �������̽��� ����Ͽ� ���� ���� �����ϴ�.
/// </summary>
class CAceGameObject : public Engine::CGameObject, public Engine::ITeamAgent
{
	DERIVED_CLASS(CGameObject, CAceGameObject)

protected:
	explicit CAceGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceGameObject(const CAceGameObject& rhs);
	virtual ~CAceGameObject();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() PURE;
	virtual void		Free() override;
};