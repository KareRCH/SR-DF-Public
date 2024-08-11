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


class CAceThrow : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceThrow)

		enum class THROW_NAME {
		// Paper (6) 
		PAPERBOX, SCRUNCHEDPAPER,FEDORA, ROPE, SACK, HANDBAG,

		// Wood (13)
		BARSTOOL, CAFESIGN, CAFETABLE, COATHANGER, TOOLBOX, UMBRELLASTAND, WOODENBARREL,
		WOODENSTOOL,  MATCHES, BOARD, MANNEQUIN, CABLEREEL, CHAIR,

		// Glass (10)
		BEERGLASS, BEERMUG, WHISKEYGlASS, POTPLANT, TABLEPLANTLEAF, TABLEPLANTFLOWER,
		MARTINIGLASS, PERFUME, GENERICBOTTLE, LAMP,

		// Steel (10)
		BUCKET, TRAFFICCONE, TRASHBIN, TRASHCAN, MICROPHONE, CHAMPAGNEBUCKET, OILBARREL, 
		TRAFFICBOLLARD, PAINTCAN, METALBARREL, 

		THROW_END };

private:
	explicit CAceThrow(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceThrow(const CAceThrow& rhs);
	virtual ~CAceThrow();

public:
	static CAceThrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag,
		const _float _fx, const _float _fy, const _float _fz, const _float iCnt = 1.f);
	static CAceThrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

private:
	virtual void		Free();
	

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:/*
	virtual void OnCollision(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) override;
	virtual void OnCollisionExited(CGameObject* pDst) override;*/
	

private: // �Լ� 
	HRESULT				Add_Component();
	HRESULT				BillBoard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	void				Height_On_Terrain();
	void				ThrowName(const _tchar* pObjTag); //������ ENUM�� ������ �����ؼ� �ѹ��� ���� 
	void				Change_Texture(THROW_NAME eReceiveName);

private: // ���� 
	THROW_NAME			m_pCurName;
	_float m_fHp = 2.f;
	_float m_fBrokenHp = 0.f;
	_float m_fCrackedHp = 1.f;
	_float m_fMaxHP = 2.f;
	_bool m_bThrow = false; // BlackBoard���� �ش� bool�� �������� = �ǰݽ� true�� �ٲ�� hp �ϳ� ���� 

	_float		m_fFrame = 0.f;						// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;						// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;						// ������ ������ �ӵ�
	OBJECT_CLASS		m_eFactoryClass;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	//CColliderComponent* m_pColliderComp = nullptr;

	
};
