#pragma once

#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
	DERIVED_CLASS(CBase, CScene)
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene();

public:
	virtual void	Free();

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene();

	virtual HRESULT		ReadyLate_Scene();

protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev;

public:
	CComponent*			Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);
	CGameObject*		Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	void				Add_GameObject(const _tchar* pLayerTag, CGameObject* const pObj);
	void				Add_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag, CGameObject* pObj);

	CLayer*				Get_Layer(const _tchar* pLayerTag) { return m_mapLayer[pLayerTag]; }
	HRESULT				Add_Layer(const _tchar* pLayerTag, CLayer* pLayer);

	HRESULT				Delete_LayerAll();

protected:	// 레이어 관련
	// 모든 레이어가 준비되었을 때 작성하는 코드입니다.
	virtual HRESULT		Ready_Layer_Completed() PURE;
	

protected:
	_unmap<wstring, CLayer*>		m_mapLayer;
	vector<CLayer*>					m_vecPriorityLayer;

};

END