#pragma once

#include "UI.h"
/// <summary>
/// 플레이어의 정보를 표시하는 UI 클래스
/// 왼쪽 하단의 버드박스와 HP 글자를 출력하게된다. 
/// </summary>
class CUI_Player : public CUI
{
	DERIVED_CLASS(CUI, CUI_Player)
protected:
	explicit CUI_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_Player(const CUI_Player& rhs);
	virtual ~CUI_Player();

public:
	static CUI_Player* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private: 
	virtual void Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;
	virtual void Render_GameObject() override;

//---------------------------------------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
//---------------------------------------------------------------------------------------------
private:
	void	Update_InternalData();
	HRESULT Add_Component();

private:
	GAUGE<_float> m_gPlayerHP;
};

