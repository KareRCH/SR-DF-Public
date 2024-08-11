#pragma once
#include "GameObject.h"

class CPlayerAnimation : public Engine::CGameObject
{
public:
    //CPlayerAnimation();
    virtual ~CPlayerAnimation();

public:
    static CPlayerAnimation* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
    virtual HRESULT Ready_GameObject();
    virtual _int	Update_GameObject(const _float& fTimeDelta);
    virtual void	LateUpdate_GameObject();
    virtual void	Render_GameObject();

public:
    virtual void Free() override;

};

