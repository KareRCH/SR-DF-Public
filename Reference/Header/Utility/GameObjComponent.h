#pragma once

#include "Component.h"

BEGIN(Engine)

/// <summary>
/// ������Ʈ�� �Ļ����� ���ӿ�����Ʈ�� �߰��Ǿ� ���̴� ������Ʈ�� �����̴�.
/// ����� �����Ǳ� ���̶� ���������� ���ӿ�����Ʈ���� ������ �ʰ� CComponent�� ����.
/// ���� �߰����� ����� �����ϱ� ���� �������� ���� ������ ������ �ӽ÷� ��������.
/// </summary>
class ENGINE_DLL CGameObjComponent abstract : public CComponent
{
	DERIVED_CLASS(CComponent, CGameObjComponent)

protected:
	explicit CGameObjComponent();
	explicit CGameObjComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObjComponent(const CGameObjComponent& rhs);
	virtual ~CGameObjComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}
};

END