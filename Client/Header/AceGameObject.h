#pragma once
#include "GameObject.h"
#include "ITeamAgent.h"


/// <summary>
/// 폴른 에이스 전용 게임 오브젝트
/// 팀 에이전트 인터페이스를 사용하여 팀을 설정 가능하다.
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

	// 성희 추가 : MeshColider(Box, SphereColider)
	virtual void		MeshBoxColider(_float _Width, _float _Height, _float Depth) override;
	virtual void		MeshSphereColider(_float Radius, _uint Slices, _uint Stacks) override;

protected:

};