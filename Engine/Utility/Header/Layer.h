#pragma once

#include "GameObject.h"

BEGIN(Engine)

/// <summary>
/// ���̾�� GameObject�� ���������� GameObject�� Update �κ��� �����ϴ� Ŭ������
/// ���� ������ ������ ó���ϴ� ���Ҹ��� �����ϴ� ���� �������� �Ѵ�.
/// </summary>
class ENGINE_DLL CLayer : public CBase
{
	DERIVED_CLASS(CBase, CLayer)
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	static CLayer*	Create(_float fPriority);

private:
	virtual void	Free();

public:
	HRESULT			Ready_Layer(_float fPriority);
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer();
	//void			Render_Layer(); �����δ� Renderer���� ��� �������� �����Ѵ�.

public:
	CComponent*		Get_Component(COMPONENTID eID, const _tchar* pObjTag, const _tchar* pComponentTag);

	

public:
	HRESULT			Add_GameObject(CGameObject* pGameObject);
	HRESULT			Add_GameObject(const wstring pObjTag, CGameObject* pGameObject);
	CGameObject*	Get_GameObject(const wstring pObjTag);

private:
	_unmap<wstring, CGameObject*>	m_mapObject;
	// ������ ���������� ���༭ �ʿ����! �׷��� ����� ������ ����
	vector<CGameObject*>			m_arrvecPriorityObject[static_cast<_uint>(EPRIORITY::RENDER)];	

public:
	GETSET_EX1(_float, m_fPriority, Priority, GET_C_REF)

protected:
	_float			m_fPriority;


};

END