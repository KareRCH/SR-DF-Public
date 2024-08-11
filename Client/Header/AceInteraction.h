#pragma once

#include "AceObjectFactory.h"
#include "AceUnit.h"
#include "Engine_Enum.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CAceInteraction :public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceInteraction)

	enum class INTERACTION_NAME {LIGHTSWITCH, ONE_BUTTON, TWO_BUTTON,  BLOCK_SWITCH, PADLOCK,
								NEWSPAPER, COMICBOOK,
								BUILDING_KEY, BOSSROOM_KEY,	INTERACTION_END };

private:
	explicit CAceInteraction(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceInteraction(const CAceInteraction& rhs);
	virtual ~CAceInteraction();

public:
	static CAceInteraction* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
	static CAceInteraction* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

private:
	virtual void		Free();											// Release

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	
public:
	virtual void OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionExited(CGameObject* pDst) override;

private: // �Լ� 
	HRESULT				Add_Component();								// ������Ʈ �߰� 
	HRESULT				BillBoard(const _float& fTimeDelta);			// ������
	void				Height_On_Terrain();							// ����Ÿ�� 
	void				ObjectName(const _tchar* pObjTag);				// ó�� �־��� wchar�� enum���� ���� 
	void				Change_Texture(INTERACTION_NAME eReceiveName);  // ����Ǵ� texture�� �ٲٴ°� 
	OBJECT_CLASS		m_eFactoryClass;

private: // ���� 
	INTERACTION_NAME			m_pCurName;								// ���� ������Ʈ enum 
	_float m_fHp = 2.f;
	_float m_fBrokenHp = 0.f;
	_float m_fCrackedHp = 1.f;
	_float m_fMaxHP = 2.f;
	_float m_fBillboardHeight = 0.f;
	_bool m_bOnOff = FALSE;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;

	
};
