#pragma once
#include "MonsterAttackUnion.h"

class CThrowPipe : public CMonsterAttackUnion
{
	DERIVED_CLASS(CMonsterAttackUnion, CThrowPipe)

private:
	explicit CThrowPipe(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CThrowPipe(const CThrowPipe& rhs);
	virtual ~CThrowPipe();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CThrowPipe* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CGameObject* pOwner, ETEAM_ID _eTeamid);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

	// �浹 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);
	
private:
	void			Dir_Setting();
	_vec3			m_vDir; // ���ư��� ���� 
	_bool			_bLoop; // �ѹ����� �������� ��� ���ư����� 
	_float			m_fMoveSpeed; // ����ü �̵����ǵ� 
};

