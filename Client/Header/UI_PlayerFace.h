#pragma once

#include "UI.h"

/// <summary>
/// 왼쪽하단 버드박스 위에 표시할 플레이어 얼굴이미지 직교투영하는 UI
/// </summary>

class CUI_PlayerFace : public CUI
{
	DERIVED_CLASS(CUI, CUI_PlayerFace)
protected:
	explicit CUI_PlayerFace(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_PlayerFace(const CUI_PlayerFace& rhs);
	virtual ~CUI_PlayerFace();

public:
	static CUI_PlayerFace* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

//---------------------------------------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)

//---------------------------------------------------------------------------------------------
private:
	void				Update_InternalData();
	HRESULT				Add_Component();
	void				Change_Texture(const _float& fTimeDelta);


private:
	GAUGE<_float>		m_gPlayerHP;
	_bool				m_bPlayerHit;
	_bool				m_bTrigger;
	_bool				m_bFaceTurn;

	_bool Random_variable(_int _winning)
	{
		//운영체제 단에서 제공하는 진짜 난수 - 컴퓨터가 주변의 환경과 무작위적으로 상호작용하면서 만드는것
		// 의사난수보다 난수를 생성하는 속도가 매우 느리다. 
		random_device rd;

		// 메르센 트위스터라는 알고리즘을 사용해서 난수 생성 엔진을 초기화
		mt19937 gen(rd());

		// 0 부터 99 까지 연속확률분포 생성   (min, max)
		uniform_int_distribution<_int> iDistribution(0, 99);

		//랜덤값 생성 
		_int RandomValue = iDistribution(gen);

		//매개변수가 5일경우 5프로 확률로 true가 반환된다
		return (RandomValue <= _winning);
	}
};

// Healthy(100 ~ 80) - SlightlyHurt (80 ~ 60) - Hurt (60~40) - Moderate Hurt (40~20) - BadlyHurt(20 ~ 0)  
// Healthy  SlightlyHurt  Hurt  ModeratelyHurt  BadlyHurt
// 평소 - normal - left - normal - right 랜덤 
// 맞은 위치에 따라 left pain 출력 

// 프레임설명
// 0 정면, 1왼쪽, 2오른쪽, 4 사이드에서 히트 

// TRI FACE
//	T	T	
//	T	F	= 중앙 바라보게 셋팅
//	F	T	= 처음으로 돌아감 - 트리거 X상태 좌/우 표정으로 
//	F	F