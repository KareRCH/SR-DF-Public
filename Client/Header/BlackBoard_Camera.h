#pragma once

#include "BlackBoard.h"

class CDynamicCamera;

/// <summary>
/// 이니셜라이즈용 구조체
/// </summary>
struct FBlackBoard_CameraInit
{
	CDynamicCamera* pCamera;
};


/// <summary>
/// 카메라 전용 블랙보드
/// </summary>
class CBlackBoard_Camera : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Camera)

protected:
	explicit CBlackBoard_Camera();
	explicit CBlackBoard_Camera(const CBlackBoard_Camera& rhs);
	virtual ~CBlackBoard_Camera();

public:
	static CBlackBoard_Camera* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// 초기화용 구조체를 만들어 매개변수로 받게 한다.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_CameraInit& tInit);

public:		// 플레이어에 대한 정보만 얻을 때는 
	GETSET_EX1(CDynamicCamera*, m_pCamera, Camera, GET)
	GETSET_EX2(_bool, m_bOne, OneSwitch, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bThree, ThreeSwitch, GET_C_REF, GET_REF)
	GETSET_EX2(_vec3, m_vEye, Eye, GET, SET)
	GETSET_EX2(_vec3, m_vAt, At, GET, SET)

private:
	CDynamicCamera* m_pCamera = nullptr;

public:		// 데이터 가공의 성격을 띌때는 해당 함수를 직접 만들것.


private:// 카메라에서 가져와 담을 그릇
	_bool		m_bThree = false;
	_bool		m_bOne = false;
	_vec3		m_vEye = { 0.f, 0.f, 0.f };
	_vec3		m_vAt = { 0.f, 0.f, 0.f };
};

