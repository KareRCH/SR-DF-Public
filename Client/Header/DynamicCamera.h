#pragma once

#include "Camera.h"

#include "Engine_Macro.h"
#include "BlackBoard_Camera.h"
#include "Player.h"

class CDynamicCamera : public Engine::CCamera
{
	DERIVED_CLASS(CCamera, CDynamicCamera)
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
									const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFov = D3DXToRadian(60.f),
									const _float& fAspect = (float)WINCX / WINCY,
									const _float& fNear = 0.1f,
									const _float& fFar = 1000.f);

private:
	virtual void Free();

public:
	HRESULT Ready_GameObject(const _vec3* pEye,
								const _vec3* pAt,
								const _vec3* pUp,
								const _float& fFov,
								const _float& fAspect,
								const _float& fNear,
								const _float& fFar);

	virtual _int Update_GameObject(const _float& fTimeDelta) override;

private:
	void		Check_KeyInput(const _float& fTimeDelta);
	void		Mouse_Move();
	void		Mouse_Fix();
	void		Camera_State(const _float& fTimeDelta);
	void		Quaternion_Ver(const _float& fTimeDelta);
	void		Update_BlackBoard();
	void		Update_InternalData();
	void		CameraAttackMove(const _float& fTimeDelta);

private:
	FBlackBoardPtr<CBlackBoard_Camera>		m_wpBlackBoard_Camera;
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;

private:
	_float		m_fSpeed = 10.f;
	_bool		m_bFix = false;
	_bool		m_bCheck = false;

	//���콺 ���� ��ǥ
	_matrix		m_matRotX;
	_matrix		m_matRotY;

	// ī�޶�
	_vec3			m_vRight;		// Right ����
	D3DXQUATERNION	m_quaternion;	// ���ʹϿ� ����

	_vec3			m_vOffset = { 0.f, 5.f, -8.f };	// �÷��̾�κ��� Offset
	_vec3			m_vOffsetOne = { 0.f, 0.f, -1.f };	// 1��Ī Offset
	_float			m_fPlayerRotationY = 0.f;
	_long			m_dwMouseMove[ROT_END];
	_float			m_fMaxAngleY;
	_float			m_fMinAngleY;
	_float			m_fAngleX;
	_float			m_fAngleY;
	_float			m_fRotSpeed = 0.2f;

	//�ҿ� �߰� 
	_bool		m_bPressC = false;
	_vec3		m_fBackupEye;

	_vec3		m_vSit;
	_tchar						debugString[100];	// ����׿� sting 


	// ���� �����ӿ��� ���콺 �̵� �� ����
	_long m_dwPrevMouseMoveX = 0;
	_long m_dwPrevMouseMoveY = 0;

private:
	//_vec3	vPlayerPosEye;			// �÷��̾��� ������ ��ġ
	_vec3	vPlayerLook;			// �÷��̾ �ٶ󺸴� ��

	_vec3 m_vRotPlus = { 0.f, 0.f, 0.f };
	_vec3 m_vRotMinus = { 0.f, 0.f, 0.f };

	_vec3 m_vCurLook = { 0.f, 0.f, 0.f };

	_bool m_bRotChange = false;
	_bool m_bRotStart = false;
	_bool m_bLRotStart = false;
	_bool* m_bLAttackMove = nullptr;
	_bool* m_bRAttackMove = nullptr;
	_bool m_bAttackOn = false;

	_float fRotStart = 0.f;
	_float fLRotStart = 0.f;


private:
	CTransformComponent* pPlayerTransCom = nullptr;
	CTransformComponent* pCameraTransCom = nullptr;

public:
	GETSET_EX2(CGameObject*, m_pTarget, Target, GET, SET)
	GETSET_EX2(_vec3, m_vAt, CameraAt, GET, SET)
		

private:
	CGameObject*	m_pTarget = nullptr;
	_float			m_fTarget_Distance = 10.f;

	STATE_PLAYER	m_ePlayer_State;
	STATE_RIGHTHAND	m_eRightHand_State;
	_bool			m_bPlayerAttackOn;

};

