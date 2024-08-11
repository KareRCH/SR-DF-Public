#pragma once
#include "AceObjectFactory.h"
#include "AceUnit.h"
#include "Engine_Enum.h"
#include "Export_System.h"
#include "Export_Utility.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CAceFood : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceFood)



private:
	explicit CAceFood(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceFood(const CAceFood& rhs);
	virtual ~CAceFood();

public:
	static CAceFood* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
	static CAceFood* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial);

	enum class FOOD_NAME {
		APPLE, BANANA, COLA, MEDIKIT,
		EATENAPPLE, BANANAPEEL,
		FOOD_END
	};

private:
	virtual void		Free();

public:
	PRIVATE virtual HRESULT		Ready_GameObject() override;
	PUBLIC	virtual HRESULT		Ready_GameObject(const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
	PUBLIC	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);

	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	
protected:
	virtual void		OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void		OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void		OnCollisionExited(CGameObject* pDst);

protected:
	HRESULT				Add_Component(); // ������Ʈ �߰� 
	HRESULT				BillBoard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	void				Height_On_Terrain(); // ����Ÿ�� 

public:
	GETSET_EX2(OBJECT_CLASS, m_eFactoryClass, FACTORY_NAME, GET, SET)
	GETSET_EX2(_bool, m_bEaten, FOOD_EAT, GET, SET)

public:
	FOOD_NAME	Get_FoodName() { return m_pCurName; }
	_float		Get_Hp() { return m_fHp; }
	_bool		m_bEaten = false;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;

private:// �Լ�
	void				FoodName(const _tchar* pObjTag); // ��������������
	void				Eat_Food(FOOD_NAME eCurName, const _float& fTimeDelta, _bool bEat);
	void				Serialize_FoodName(wstring _ObjName); // �ø���ѹ��� 

private: //����
	FOOD_NAME			m_pCurName;
	_bool				m_bDead = false;  // �״°� ����� �װɷ� ���� 
	_vec3				CurPos;			  // ���� ��ġ 
	_float				m_fChangeTextureSpeed = 0.001f;
	OBJECT_CLASS		m_eFactoryClass;
	_float				m_fHp;

};

