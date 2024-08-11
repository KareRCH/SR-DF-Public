#include "stdafx.h"
#include "DynamicCamera.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "TransformComponent.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
	Set_ObjectName(L"DynamicCamera");
}

CDynamicCamera::~CDynamicCamera()
{
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		MSG_BOX("DynamicCamera Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDynamicCamera::Free()
{
	SUPER::Free();
}


HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	m_vEye = m_fBackupEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;
	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	// ��� ������Ʈ
	NULL_CHECK_RETURN(pCameraTransCom = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	ShowCursor(false);
	m_bOne = true;

#pragma region ������

	Engine::Add_BlackBoard(L"Camera", CBlackBoard_Camera::Create());

#pragma endregion

	return S_OK;
}


_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	// #���� ī�޶� �̻��ϴٸ� ���� ī�޶�� �����ϼ���

	// ������ ���ε�
	Update_BlackBoard();

	// ������ �ٿ�ε�
	Update_InternalData();

	Check_KeyInput(fTimeDelta);

	// ���� ī�޶�
	Camera_State(fTimeDelta);

	// ���ʹϿ� ���� ī�޶�
	//Quaternion_Ver(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}

	//// ī�޶� ���� �÷��̾� ����
	//if (m_pTarget)
	//{
	//	CTransformComponent* pTransform = dynamic_cast<CTransformComponent*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	//	_vec3 vPos;
	//	pTransform->Get_Info(INFO_POS, &vPos);
	//	m_vAt = vPos;
	//}

	//if (m_bPressC)
	//{
	//	if (STATE_PLAYER::SITDOWN == m_ePlayer_State) //�ɾ��ִ»��� = �Ͼ
	//	{
	//		_vec3	vLength = *D3DXVec3Normalize(&m_vSit, &m_vSit) * -m_fSpeed * fTimeDelta;

	//		m_vEye += vLength;

	//		if (m_vEye.y <= 1.1f)
	//		{
	//			m_vEye.y = 1.1f;
	//			m_bPressC = false;
	//		}
	//	}
		//else // �ɱ�
		//{
		//	_vec3	vLength = *D3DXVec3Normalize(&m_vSit, &m_vSit) * -m_fSpeed * fTimeDelta;

		//	m_vEye += vLength;

		//	if (m_vEye.y <= 1.1) //m_fBackupEye.y * 0.5 = 2.5
		//	{
		//		m_vEye.y = 1.1;
		//		m_bPressC = false;
		//	}
		//}
	//}


	//swprintf_s(debugString, L"�ں���üũ ī�޶�y  = %f \n", m_vEye.y);
	//OutputDebugStringW(debugString);

	SUPER::Update_GameObject(fTimeDelta);


	return 0;
}

void CDynamicCamera::Check_KeyInput(const _float& fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, nullptr, &m_matView);

	if (Engine::IsKey_Pressing(DIK_UP))
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::IsKey_Pressing(DIK_DOWN))
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::IsKey_Pressing(DIK_LEFT))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::IsKey_Pressing(DIK_RIGHT))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::IsKey_Pressed(DIK_C)) // �ɱ� �߰�
	{
		memcpy(&m_vSit, &matCamWorld.m[1][0], sizeof(_vec3));
		m_bPressC = true;
	}


	//if (m_bPressC)
	//{
	//	if (true == m_bWakeUp) //�Ͼ
	//	{
	//		m_vEye += vLength;
	//		m_bPressC = false;
	//	}
	//	else if (false == m_bWakeUp) // �ɱ� 
	//	{
	//		m_bPressC = false;
	//	}
	//}
	//if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
	//{
	//	_vec3	vUp;
	//	memcpy(&vUp, &matCamWorld.m[1][0], sizeof(_vec3));

	//	_vec3	vLength = *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;

	//	m_vEye -= vLength;
	//	m_vAt -= vLength;
	//}

	//if (Engine::Get_DIKeyState(DIK_E) & 0x80)
	//{
	//	_vec3	vUp;
	//	memcpy(&vUp, &matCamWorld.m[1][0], sizeof(_vec3));

	//	_vec3	vLength = *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;

	//	m_vEye += vLength;
	//	m_vAt += vLength;
	//}

	// 1��Ī
	if (Engine::IsKey_Pressed(DIK_F1))
	{
		if (m_bThree)
			m_bThree = false;

		if (!m_bOne)
		{
			m_bOne = true;
		}
	}

	// 3��Ī
	if (Engine::IsKey_Pressed(DIK_F2))
	{
		if (m_bOne)
			m_bOne = false;

		if (!m_bThree)
		{
			m_bThree = true;
		}
	}

	// ���� ����
	if (Engine::IsKey_Pressed(DIK_F3))
	{
		if (m_bOne)
			m_bOne = false;

		if (m_bThree)
		{
			m_bThree = false;
		}
	}

	// �� Ű�� ���� ���콺 Fix�� �״� ���� ��
	if (Engine::IsKey_Pressed(DIK_TAB))
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}
	else
		m_bCheck = false;

	if (false == m_bFix)
		return;
}
void CDynamicCamera::Mouse_Move()
{
	// ���콺�� �̵��ϴ� ����
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	_long	dwMouseMove = 0;

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));
		_vec3		vLook = m_vAt - m_vEye;
		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		_vec3	vUp = { 0.f, 1.f, 0.f };
		_vec3		vLook = m_vAt - m_vEye;
		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
		m_vAt = m_vEye + vLook;
	}
}

void CDynamicCamera::Mouse_Fix()
{
	POINT	pt{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

// ī�޶� ���� (1��Ī, 3��Ī, ���� ����)
void CDynamicCamera::Camera_State(const _float& fTimeDelta)
{
	// �÷��̾� Trans ������Ʈ �޾ƿ���
	pPlayerTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	if (pPlayerTransCom == nullptr)
		return;

	_vec3	vPlayerPos;				// �÷��̾� ��ġ

	_vec3	vPlayerCameraPos;		// 3��Ī ī�޶� ��ġ ����(Eye)

	_vec3	vDist = { 0, 5, -8 };	// ī�޶� �÷��̾�� ���� �־����� �Ÿ�

	_vec3	vEventCameraMove = { 0.f, 0.f, 0.f };	// Ư�� �ൿ�� ī�޶��� ������


	_matrix	matPlayerRotX;		// �÷��̾� ȸ�� ���� x
	_matrix	matPlayerRotY;		// �÷��̾� ȸ�� ���� y
	_matrix matPlayerRot;		// ���������� �÷��̾ ȸ���� ����

	// �÷��̾��� ��ġ, �ٶ󺸴� ������ ���´�.
	pPlayerTransCom->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTransCom->Get_Info(INFO_LOOK, &vPlayerLook);

	// �÷��̾��� ȸ���� ������ ���´�.
	float fX = pPlayerTransCom->Get_Rotation().x;
	float fY = pPlayerTransCom->Get_Rotation().y;

	// �÷��̾� ȸ�������� ��ķ� �����ش�.
	D3DXMatrixRotationX(&matPlayerRotX, fX);
	D3DXMatrixRotationY(&matPlayerRotY, fY);

	// �÷��̾� ȸ�� ��� X, Y �� ���� ��Ŀ� ���� �ϼ���Ų��. 
	D3DXMatrixMultiply(&matPlayerRot, &matPlayerRotX, &matPlayerRotY);

	// �÷��̾ �ٶ󺸴� ������ ����ȭ ���ش�.
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	//							ī�޶� ��ġ, ������ �Ÿ�, �÷��̾� ����
	D3DXVec3TransformCoord(&vPlayerCameraPos, &vDist, &matPlayerRot);

	// �÷��̾�� ���� ī�޶� ������ �Ÿ��� ���Ѵ�.
	vPlayerCameraPos = vPlayerPos + vPlayerCameraPos;

	if (Engine::IsKey_Pressing(DIK_K))
	{
		vEventCameraMove.y += 0.1f;
	}
	if (Engine::IsKey_Pressing(DIK_J))
	{
		vEventCameraMove.y -= 0.1f;
	}

	// 3��Ī
	if (m_bThree)
	{
		// ī�޶� ��ġ ����
		m_vEye = { vPlayerCameraPos.x, vPlayerCameraPos.y, vPlayerCameraPos.z };

		// ī�޶� �ٶ󺸴� ��� (�÷��̾�)
		m_vAt = vPlayerPos;
	}


	// 1��Ī
	if (m_bOne)
	{
		// ī�޶� ��ġ ����
		vPlayerPos.y += 0.7f;

		m_vEye = vPlayerPos; // �÷��̾��� ������

		// ī�޶� �ٶ󺸴� ��� (�÷��̾ �ٶ󺸴� ����)
		m_vAt = vPlayerPos + vPlayerLook;// +vEventCameraMove;

		Mouse_Move(); // ���콺 �̵�
		Mouse_Fix();  // ���콺 ����
	}

	//// �÷��̾� ���� ī�޶� ����
	//if (m_bAttackOn)
	//{
	//	CameraAttackMove(fTimeDelta);

	//	//// ī�޶� ��ġ ����
	//	//m_vEye = vPlayerPosEye; // �÷��̾��� ������

	//	Mouse_Move(); // ���콺 �̵�
	//	Mouse_Fix();  // ���콺 ����
	//}
	//else
	//{
	//	// 1��Ī
	//	if (m_bOne)
	//	{
	//		// ī�޶� ��ġ ����
	//		m_vEye = vPlayerPosEye; // �÷��̾��� ������

	//		// ī�޶� �ٶ󺸴� ��� (�÷��̾ �ٶ󺸴� ����)
	//		m_vAt = vPlayerPosEye + vPlayerLook;// +vEventCameraMove;

	//		Mouse_Move(); // ���콺 �̵�
	//		Mouse_Fix();  // ���콺 ����
	//	}
	//}
}

void CDynamicCamera::Quaternion_Ver(const _float& fTimeDelta)
{
	// �÷��̾� Trans ������Ʈ �޾ƿ���
	pPlayerTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));

	_vec3	vPlayerPos;				// �÷��̾� ��ġ
	_vec3	vPlayerLook;			// �÷��̾ �ٶ󺸴� ��
	_vec3	vPlayerForward;			// �÷��̾��� �̵� ���� ����

	// �÷��̾��� ��ġ, �ٶ󺸴� ������ ���´�.
	pPlayerTransCom->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTransCom->Get_Info(INFO_LOOK, &vPlayerLook);
	//pPlayerTransCom->Rotation();

	_vec3 vOffSet = { 0.f, 5.f, -10.f };

	D3DXQUATERNION quatPitch, quatYaw, quatRoll;

	_long dwMouseMove = 0;
	_long dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
	_long dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

	// ī�޶��� Right���� ���ϱ�
	// vForward = ��ǥ�� - ī�޶�
	_vec3 vForward = pPlayerTransCom->Get_Pos() - m_vEye;
	//vForward�� ������ up����(0,1,0)�� �����Ͽ� Right���͸� ���Ѵ�.
	D3DXVec3Cross(&m_vRight, &m_vUp, &vForward);

	// 1��Ī
	if (m_bOne)
	{
		// ���콺�� �̵��ϴ� ����
		_matrix		matCamWorld;
		D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

		if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
		{
			_vec3	vRight;
			memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));
			_vec3		vLook = m_vAt - m_vEye;
			_matrix		matRot;

			D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
			m_vAt = m_vEye + vLook;
		}

		if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
		{
			_vec3	vUp = { 0.f, 1.f, 0.f };
			_vec3		vLook = m_vAt - m_vEye;
			_matrix		matRot;

			D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
			m_vAt = m_vEye + vLook;
		}

		// �÷��̾��� �̵� ���͸� ����Ͽ� ī�޶� ��ġ ����
		m_vEye = vPlayerPos;

		//// ī�޶� �ü� ��ġ ���
		//m_vAt = vPlayerPos + vPlayerLook;

		// 1��Ī ����� ��� ī�޶� ��ġ�� �ü��� �÷��̾� ��ġ�� ��ġ��Ų��.
		// �ü��� �÷��̾��� ��ġ + �÷��̾ �ٶ󺸴� ����. �� �÷��̾ �ٶ󺸴� ����
		//m_vEye = pPlayerTransCom->m_vInfo[INFO_POS];
		//m_vAt = pPlayerTransCom->m_vInfo[INFO_POS] + pPlayerTransCom->m_vInfo[INFO_LOOK];
	}
	// 3��Ī (3��Ī ����� ��� ī�޶� �÷��̾� �������� ȸ����Ų��. [����])
	if (m_bThree)
	{
		// ī�޶��� Look ���� ����
		_vec3 vLook = { 0.f, 0.f, 0.f };

		// ���ʹϾ� ȸ���� ���� Yaw, Pitch, Roll ����
		D3DXQUATERNION quatYaw, quatPitch, quatRoll;

		// ���콺 �������� ���� ���� ���� �� ��������
		_long dwMouseMove = 0;
		m_dwMouseMove[ROT_X] = Engine::Get_DIMouseMove(DIMS_X);
		m_dwMouseMove[ROT_Y] = Engine::Get_DIMouseMove(DIMS_Y);


		// ī�޶��� Right���� ���ϱ�
		vLook = pPlayerTransCom->Get_Pos() - m_vEye;      // m_vLook = ��ǥ�� - ī�޶�
		//m_vLook�� ������ up����(0,1,0)�� �����Ͽ� Right���͸� ���Ѵ�.
		D3DXVec3Cross(&m_vRight, &m_vUp, &vLook);

		// �ޱ۰� ������ ���� �ּ�, �ִ� �� ����
		m_fMaxAngleY = 150.f;
		m_fMinAngleY = 90.f;

		// ī�޶� ��ġ�� �÷��̾� Pos + ������ �Ÿ�
		m_vEye = pPlayerTransCom->Get_Pos() + m_vOffset;

		// �ٶ󺸴� ����� �÷��̾�
		m_vAt = pPlayerTransCom->Get_Pos();

		// ���� ȸ�� (Y�� ȸ��)
		if (dwMouseMoveX != 0)
		{
			// ���ʹϾ� ȸ�� ����
			D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));
			// ���ʹϾ� �� ���ֱ�
			m_quaternion *= quatYaw;
			// ���ʹϾ� ����ȭ
			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		}

		// ���� ȸ�� (X�� ȸ��)
		if (dwMouseMoveY != 0)
		{
			// ���ʹϾ� ȸ�� ����
			D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));
			// ���ʹϾ� �� ���ֱ�
			m_quaternion *= quatPitch;
			// ���ʹϾ� ����ȭ
			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		}

		// ���� ���콺 �Է� ���� �����Ͽ� ���� �����ӿ� ���
		m_dwPrevMouseMoveX = dwMouseMoveX;
		m_dwPrevMouseMoveY = dwMouseMoveY;

		// ���� ���콺 ����� ȸ���ؼ� ������
#pragma region ��Ŭ�� ȸ��
		//if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
		//{
		//	_float fNewAngle = m_fAngleY + (m_dwMouseMove[ROT_Y] * m_fRotSpeed);

		//	if (fNewAngle < m_fMaxAngleY && fNewAngle > m_fMinAngleY)
		//	{
		//		if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
		//		{
		//			D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(m_dwMouseMove[ROT_Y] * m_fRotSpeed));
		//			m_quaternion *= quatPitch;
		//			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		//		}
		//	}

		//	// ���� ȸ�� (Y�� ȸ��)
		//	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
		//	{
		//		D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(m_dwMouseMove[ROT_X] * m_fRotSpeed));
		//		m_quaternion *= quatYaw;
		//		D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		//	}
		//}
#pragma endregion

		_matrix matRotation; // ȸ�� ����� ����� ���� ���
		D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion); // ���ʹϾ��� ȸ�� ��ķ� ��ȯ

		_vec3 vPos = m_vEye - pPlayerTransCom->Get_Pos();
		// ��� ���� ������ ���� �ּ�, ��İ� ���ϱ⸦ �� ��ġ ���� �ּ�, ��� �ּ�;
		D3DXVec3TransformCoord(&vPos, &vPos, &matRotation);      // ��ġ���� 

		m_vEye = vPos + pPlayerTransCom->Get_Pos();

	}
}

void CDynamicCamera::Update_BlackBoard()
{
	// ������ ���� ���, ���� �ڵ�� �ʼ�
	if (!m_wpBlackBoard_Camera.Get_BlackBoard())
	{
		m_wpBlackBoard_Camera.Set_BlackBoard(Engine::Get_BlackBoard(L"Camera"));
		// ���� ����
		if (!m_wpBlackBoard_Camera.Get_BlackBoard())
			return;
	}

	// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	CBlackBoard_Camera* pBlackBoard = m_wpBlackBoard_Camera.Get_BlackBoard();

	// ���⼭���� �������� ������ ������Ʈ �Ѵ�.
	pBlackBoard->Get_OneSwitch() = m_bOne;
	pBlackBoard->Get_ThreeSwitch() = m_bThree;
	pBlackBoard->Set_Eye(m_vEye);
	pBlackBoard->Set_At(m_vAt);

}

// ������ ���� �����ޱ� (���� �����ޱ�)
void CDynamicCamera::Update_InternalData()
{
	// ������ ���� ���, ���� �ڵ�� �ʼ�
	if (!m_wpBlackBoard_Player.Get_BlackBoard())
	{
		m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
		// ���� ����
		if (!m_wpBlackBoard_Player.Get_BlackBoard())
			return;
	}

	// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	CBlackBoard_Player* m_pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

	// ���⼭���� �������� ������ ���´�.
	m_ePlayer_State = m_pBlackBoard->Get_PlayerState();
	m_eRightHand_State = m_pBlackBoard->Get_RightHandState();
	m_bPlayerAttackOn = m_pBlackBoard->Get_AttackOn();
	m_bRAttackMove = &m_pBlackBoard->Get_RAttackMove();
	m_bLAttackMove = &m_pBlackBoard->Get_LAttackMove();
	m_bAttackOn = m_pBlackBoard->Get_AttackOn();

	//m_bRAttackMove
}

void CDynamicCamera::CameraAttackMove(const _float& fTimeDelta)
{
	_vec3 vLook = { 0.f, 0.f, 0.f };
	_vec3 vRight = { 0.f, 0.f, 0.f };
	_vec3 vUp = { 0.f, 0.f, 0.f };

	_vec3 vCrossUp = { 0.f, 0.f, 0.f };

	_vec3 vPosEye = { 0.f, 0.f, 0.f };

	//pPlayerTransCom->Get_Info(INFO_LOOK, &vLook);
	//pPlayerTransCom->Get_Info(INFO_RIGHT, &vRight);
	//pPlayerTransCom->Get_Info(INFO_UP, &vUp);
	//vPosEye = pPlayerTransCom->Get_Pos();// +pPlayerTransCom->Get_Pos() / 2;
	vUp = pPlayerTransCom->Get_Up();
	vLook = pPlayerTransCom->Get_Look();
	vPosEye = pCameraTransCom->Get_Pos();
	//vPosEye = pPlayerTransCom->Get_PosEye();

	// ī�޶� ��ġ ����
	m_vEye = vPosEye; // �÷��̾��� ������

	switch (m_eRightHand_State)
	{
	case Engine::STATE_RIGHTHAND::NONE:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::HAND: // �ָ�
	{
		if (*m_bLAttackMove)
		{
			m_bLRotStart = true;

			//m_vRot = m_pTransformComp->Get_Look();
			//m_vCurLook = m_pTransformComp->Get_Look();
			//D3DXVec3Normalize(&m_vRot, &m_vRot);
		}
		else if (*m_bRAttackMove)
		{
			m_bRotStart = true;
		}

		if (m_bLRotStart)
		{
			fLRotStart += 1.f * fTimeDelta;

			_float fMoveSpeed = 1.5f;
			_float fMoveRange = 0.2f;
			_matrix		matRot;

			if (fLRotStart > 0.3f)
			{
				if (m_vRotPlus.x < fMoveRange && !m_bRotChange)
				{
					m_vRotPlus.x += fMoveSpeed * fTimeDelta;

					// ī�޶��� up ����(y��) �������� ȸ��
					//D3DXMatrixRotationAxis(&matRot, &m_vUp, D3DXToRadian(m_vRotPlus.x));

					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotPlus.x));
					// ȸ�� ��� ���� �÷��̾��� Look���ͷ� ���⺤�� ����
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// �������� ���� ���⺤�͸� ����
					m_vAt = vPosEye + vLook;// +vEventCameraMove;
					cout << vPosEye.x << endl;
					cout << vPosEye.y << endl;
					cout << vPosEye.z << endl;
					cout << "����" << endl;
				}
				else if (m_vRotPlus.x > fMoveRange && !m_bRotChange)
				{
					//m_vRot.y = 0.f;
					m_bRotChange = true;
				}

				if (m_bRotChange && m_vRotMinus.x > -fMoveRange)
				{
					m_vRotMinus.x -= fMoveSpeed * fTimeDelta;

					// ī�޶��� up ����(y��) �������� ȸ��
					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotMinus.x));
					// ȸ�� ��� ���� �÷��̾��� Look���ͷ� ���⺤�� ����
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// �������� ���� ���⺤�͸� ����
					m_vAt = vPosEye + vLook;// +vEventCameraMove;
				}

				if (m_vRotMinus.x <= -fMoveRange)
				{
					m_vRotMinus.x = 0.f;
					m_vRotPlus.x = 0.f;
					fLRotStart = 0.f;
					vPlayerLook.x;

					m_bRotChange = false;
					m_bLRotStart = false;

					*m_bLAttackMove = false;

					_bool test1 = *m_bLAttackMove;
					_bool test2 = m_bLAttackMove;
					int test3 = *m_bLAttackMove;
					_bool* test4 = m_bLAttackMove;
					int test = 0;

				}
			}
		}

		// ������ �ָ�
		if (m_bRotStart)
		{
			fRotStart += 1.f * fTimeDelta;

			_float fMoveSpeed = 1.5f;
			_float fMoveRange = 0.2f;
			_matrix		matRot;

			if (fRotStart > 0.3f)
			{
				if (m_vRotMinus.x > -fMoveRange && !m_bRotChange)
				{
					m_vRotMinus.x -= fMoveSpeed * fTimeDelta;

					//pPlayerTransCom->Get_Info(INFO_LOOK, &vLook);
					//pPlayerTransCom->Get_Info(INFO_RIGHT, &vRight);
					//pPlayerTransCom->Get_Info(INFO_UP, &vUp);
					//vPosEye = pPlayerTransCom->Get_PosEye();

					//D3DXVec3Cross(&vCrossUp, &vLook, &vRight);

					// ī�޶��� up ����(y��) �������� ȸ��
					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotMinus.x));
					// ȸ�� ��� ���� �÷��̾��� Look���ͷ� ���⺤�� ����
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// �������� ���� ���⺤�͸� ����
					m_vAt = vPosEye + vLook;// +vEventCameraMove;
				}
				else if (m_vRotMinus.x < -fMoveRange && !m_bRotChange)
				{
					//m_vRot.y = 0.f;
					m_bRotChange = true;
				}

				if (m_bRotChange && m_vRotPlus.x < fMoveRange)
				{
					m_vRotPlus.x += fMoveSpeed * fTimeDelta;

					//pPlayerTransCom->Get_Info(INFO_LOOK, &vLook);
					//pPlayerTransCom->Get_Info(INFO_RIGHT, &vRight);
					//pPlayerTransCom->Get_Info(INFO_UP, &vUp);
					//vPosEye = pPlayerTransCom->Get_PosEye();

					//D3DXVec3Cross(&vCrossUp, &vLook, &vRight);

					// ī�޶��� up ����(y��) �������� ȸ��
					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotPlus.x));
					// ȸ�� ��� ���� �÷��̾��� Look���ͷ� ���⺤�� ����
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// �������� ���� ���⺤�͸� ����
					m_vAt = vPosEye + vLook;// +vEventCameraMove;
				}

				if (m_vRotPlus.x >= fMoveRange)
				{
					m_vRotMinus.x = 0.f;
					m_vRotPlus.x = 0.f;
					fRotStart = 0.f;

					m_bRotChange = false;
					m_bRotStart = false;
					*m_bRAttackMove = false;
				}
			}
		}
		break;
	}
	case Engine::STATE_RIGHTHAND::RUN_HAND:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::GUN:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::THOMPSON:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::STEELPIPE:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::BEERBOTLE:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::FRYINGPAN:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::KICK:
	{

		break;
	}
	}

	//// ī�޶� ��ġ ����
	//m_vEye = vPlayerPosEye; // �÷��̾��� ������

	//// ī�޶� �ٶ󺸴� ��� (�÷��̾ �ٶ󺸴� ����)
	//m_vAt = vPlayerPosEye + vPlayerLook;// +vEventCameraMove;

	//Mouse_Move(); // ���콺 �̵�
	//Mouse_Fix();  // ���콺 ����
}

