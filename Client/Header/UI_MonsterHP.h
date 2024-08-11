#pragma once
#include "UI.h"

/// <summary>
/// 몬스터 hp출력하는 UI
/// </summary>
/// 
class CUI_MonsterHP : public CUI
{
	DERIVED_CLASS(CUI, CUI_MonsterHP)
protected:
	explicit CUI_MonsterHP(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_MonsterHP(const CUI_MonsterHP& rhs);
	virtual ~CUI_MonsterHP();

public:
	static CUI_MonsterHP* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;
	virtual void Render_GameObject() override;

private:
	CTextureComponent* m_pHPTextureComp = nullptr;
	//CTextureComponent* m_pPreHpTextureComp = nullptr;
	
	//---------------------------------------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pHPTextureComp, HpTextureComponent, GET, SET)
	//GETSET_EX2(CTextureComponent*, m_pPreHpTextureComp, PreHpTextureComponent, GET, SET)
	//---------------------------------------------------------------------------------------------
private:
	void		Update_InternalData();
	HRESULT		Add_Component();

private:
	GAUGE<_float>				m_gMonsterHP;
	CGameObject*				m_pHPOwner;
	wstring						 m_sOwner ;

	_float 	m_fBarSizeX;
	_float 	m_fBarSizeY;
	_float 	m_fRedSizeX;
	_float 	m_fRedSizeY;

	//_float 	m_fPreRedSizeX;
	_vec2	vFontPos;
	// 디버그용 sting 
	_tchar						debugString[256];
	wchar_t						debugStringOwner[256];
};


//swprintf_s(debugString, L"Gray - 변수 확인 m_fAwareness = %f\n", m_fAwareness);
//OutputDebugStringW(debugString);