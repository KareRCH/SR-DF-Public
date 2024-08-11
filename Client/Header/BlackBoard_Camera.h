#pragma once

#include "BlackBoard.h"

class CDynamicCamera;

/// <summary>
/// �̴ϼȶ������ ����ü
/// </summary>
struct FBlackBoard_CameraInit
{
	CDynamicCamera* pCamera;
};


/// <summary>
/// ī�޶� ���� ������
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
	// �ʱ�ȭ�� ����ü�� ����� �Ű������� �ް� �Ѵ�.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_CameraInit& tInit);

public:		// �÷��̾ ���� ������ ���� ���� 
	GETSET_EX1(CDynamicCamera*, m_pCamera, Camera, GET)
	GETSET_EX2(_bool, m_bOne, OneSwitch, GET_C_REF, GET_REF)
	GETSET_EX2(_bool, m_bThree, ThreeSwitch, GET_C_REF, GET_REF)
	GETSET_EX2(_vec3, m_vEye, Eye, GET, SET)
	GETSET_EX2(_vec3, m_vAt, At, GET, SET)

private:
	CDynamicCamera* m_pCamera = nullptr;

public:		// ������ ������ ������ �鶧�� �ش� �Լ��� ���� �����.


private:// ī�޶󿡼� ������ ���� �׸�
	_bool		m_bThree = false;
	_bool		m_bOne = false;
	_vec3		m_vEye = { 0.f, 0.f, 0.f };
	_vec3		m_vAt = { 0.f, 0.f, 0.f };
};

