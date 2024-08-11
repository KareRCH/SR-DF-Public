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

	// 행렬 컴포넌트
	NULL_CHECK_RETURN(pCameraTransCom = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

	ShowCursor(false);
	m_bOne = true;

#pragma region 블랙보드

	Engine::Add_BlackBoard(L"Camera", CBlackBoard_Camera::Create());

#pragma endregion

	return S_OK;
}


_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	// #만약 카메라가 이상하다면 기존 카메라로 변경하세요

	// 블랙보드 업로드
	Update_BlackBoard();

	// 블랙보드 다운로드
	Update_InternalData();

	Check_KeyInput(fTimeDelta);

	// 기존 카메라
	Camera_State(fTimeDelta);

	// 쿼터니온 적용 카메라
	//Quaternion_Ver(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}

	//// 카메라 시점 플레이어 고정
	//if (m_pTarget)
	//{
	//	CTransformComponent* pTransform = dynamic_cast<CTransformComponent*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	//	_vec3 vPos;
	//	pTransform->Get_Info(INFO_POS, &vPos);
	//	m_vAt = vPos;
	//}

	//if (m_bPressC)
	//{
	//	if (STATE_PLAYER::SITDOWN == m_ePlayer_State) //앉아있는상태 = 일어남
	//	{
	//		_vec3	vLength = *D3DXVec3Normalize(&m_vSit, &m_vSit) * -m_fSpeed * fTimeDelta;

	//		m_vEye += vLength;

	//		if (m_vEye.y <= 1.1f)
	//		{
	//			m_vEye.y = 1.1f;
	//			m_bPressC = false;
	//		}
	//	}
		//else // 앉기
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


	//swprintf_s(debugString, L"★변수체크 카메라y  = %f \n", m_vEye.y);
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

	if (Engine::IsKey_Pressed(DIK_C)) // 앉기 추가
	{
		memcpy(&m_vSit, &matCamWorld.m[1][0], sizeof(_vec3));
		m_bPressC = true;
	}


	//if (m_bPressC)
	//{
	//	if (true == m_bWakeUp) //일어남
	//	{
	//		m_vEye += vLength;
	//		m_bPressC = false;
	//	}
	//	else if (false == m_bWakeUp) // 앉기 
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

	// 1인칭
	if (Engine::IsKey_Pressed(DIK_F1))
	{
		if (m_bThree)
			m_bThree = false;

		if (!m_bOne)
		{
			m_bOne = true;
		}
	}

	// 3인칭
	if (Engine::IsKey_Pressed(DIK_F2))
	{
		if (m_bOne)
			m_bOne = false;

		if (!m_bThree)
		{
			m_bThree = true;
		}
	}

	// 자유 시점
	if (Engine::IsKey_Pressed(DIK_F3))
	{
		if (m_bOne)
			m_bOne = false;

		if (m_bThree)
		{
			m_bThree = false;
		}
	}

	// 텝 키를 눌러 마우스 Fix를 켰다 껐다 함
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
	// 마우스로 이동하는 동작
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

// 카메라 상태 (1인칭, 3인칭, 자유 시점)
void CDynamicCamera::Camera_State(const _float& fTimeDelta)
{
	// 플레이어 Trans 컴포넌트 받아오기
	pPlayerTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	if (pPlayerTransCom == nullptr)
		return;

	_vec3	vPlayerPos;				// 플레이어 위치

	_vec3	vPlayerCameraPos;		// 3인칭 카메라 위치 벡터(Eye)

	_vec3	vDist = { 0, 5, -8 };	// 카메라가 플레이어로 부터 멀어지는 거리

	_vec3	vEventCameraMove = { 0.f, 0.f, 0.f };	// 특정 행동시 카메라의 움직임


	_matrix	matPlayerRotX;		// 플레이어 회전 각도 x
	_matrix	matPlayerRotY;		// 플레이어 회전 각도 y
	_matrix matPlayerRot;		// 최종적으로 플레이어가 회전한 각도

	// 플레이어의 위치, 바라보는 방향을 얻어온다.
	pPlayerTransCom->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTransCom->Get_Info(INFO_LOOK, &vPlayerLook);

	// 플레이어의 회전한 각도를 얻어온다.
	float fX = pPlayerTransCom->Get_Rotation().x;
	float fY = pPlayerTransCom->Get_Rotation().y;

	// 플레이어 회전각도를 행렬로 곱해준다.
	D3DXMatrixRotationX(&matPlayerRotX, fX);
	D3DXMatrixRotationY(&matPlayerRotY, fY);

	// 플레이어 회전 행렬 X, Y 를 최종 행렬에 곱해 완성시킨다. 
	D3DXMatrixMultiply(&matPlayerRot, &matPlayerRotX, &matPlayerRotY);

	// 플레이어가 바라보는 방향을 정규화 해준다.
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	//							카메라 위치, 떨어질 거리, 플레이어 각도
	D3DXVec3TransformCoord(&vPlayerCameraPos, &vDist, &matPlayerRot);

	// 플레이어로 부터 카메라가 떨어진 거리를 구한다.
	vPlayerCameraPos = vPlayerPos + vPlayerCameraPos;

	if (Engine::IsKey_Pressing(DIK_K))
	{
		vEventCameraMove.y += 0.1f;
	}
	if (Engine::IsKey_Pressing(DIK_J))
	{
		vEventCameraMove.y -= 0.1f;
	}

	// 3인칭
	if (m_bThree)
	{
		// 카메라 위치 설정
		m_vEye = { vPlayerCameraPos.x, vPlayerCameraPos.y, vPlayerCameraPos.z };

		// 카메라가 바라보는 대상 (플레이어)
		m_vAt = vPlayerPos;
	}


	// 1인칭
	if (m_bOne)
	{
		// 카메라 위치 설정
		vPlayerPos.y += 0.7f;

		m_vEye = vPlayerPos; // 플레이어의 눈높이

		// 카메라가 바라보는 대상 (플레이어가 바라보는 방향)
		m_vAt = vPlayerPos + vPlayerLook;// +vEventCameraMove;

		Mouse_Move(); // 마우스 이동
		Mouse_Fix();  // 마우스 고정
	}

	//// 플레이어 공격 카메라 무빙
	//if (m_bAttackOn)
	//{
	//	CameraAttackMove(fTimeDelta);

	//	//// 카메라 위치 설정
	//	//m_vEye = vPlayerPosEye; // 플레이어의 눈높이

	//	Mouse_Move(); // 마우스 이동
	//	Mouse_Fix();  // 마우스 고정
	//}
	//else
	//{
	//	// 1인칭
	//	if (m_bOne)
	//	{
	//		// 카메라 위치 설정
	//		m_vEye = vPlayerPosEye; // 플레이어의 눈높이

	//		// 카메라가 바라보는 대상 (플레이어가 바라보는 방향)
	//		m_vAt = vPlayerPosEye + vPlayerLook;// +vEventCameraMove;

	//		Mouse_Move(); // 마우스 이동
	//		Mouse_Fix();  // 마우스 고정
	//	}
	//}
}

void CDynamicCamera::Quaternion_Ver(const _float& fTimeDelta)
{
	// 플레이어 Trans 컴포넌트 받아오기
	pPlayerTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));

	_vec3	vPlayerPos;				// 플레이어 위치
	_vec3	vPlayerLook;			// 플레이어가 바라보는 곳
	_vec3	vPlayerForward;			// 플레이어의 이동 방향 벡터

	// 플레이어의 위치, 바라보는 방향을 얻어온다.
	pPlayerTransCom->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTransCom->Get_Info(INFO_LOOK, &vPlayerLook);
	//pPlayerTransCom->Rotation();

	_vec3 vOffSet = { 0.f, 5.f, -10.f };

	D3DXQUATERNION quatPitch, quatYaw, quatRoll;

	_long dwMouseMove = 0;
	_long dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
	_long dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

	// 카메라의 Right벡터 구하기
	// vForward = 목표물 - 카메라
	_vec3 vForward = pPlayerTransCom->Get_Pos() - m_vEye;
	//vForward와 월드의 up벡터(0,1,0)를 외적하여 Right벡터를 구한다.
	D3DXVec3Cross(&m_vRight, &m_vUp, &vForward);

	// 1인칭
	if (m_bOne)
	{
		// 마우스로 이동하는 동작
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

		// 플레이어의 이동 벡터를 사용하여 카메라 위치 갱신
		m_vEye = vPlayerPos;

		//// 카메라 시선 위치 계산
		//m_vAt = vPlayerPos + vPlayerLook;

		// 1인칭 모드인 경우 카메라 위치와 시선을 플레이어 위치와 일치시킨다.
		// 시선은 플레이어의 위치 + 플레이어가 바라보는 방향. 즉 플레이어가 바라보는 방향
		//m_vEye = pPlayerTransCom->m_vInfo[INFO_POS];
		//m_vAt = pPlayerTransCom->m_vInfo[INFO_POS] + pPlayerTransCom->m_vInfo[INFO_LOOK];
	}
	// 3인칭 (3인칭 모드인 경우 카메라를 플레이어 기준으로 회전시킨다. [공전])
	if (m_bThree)
	{
		// 카메라의 Look 벡터 선언
		_vec3 vLook = { 0.f, 0.f, 0.f };

		// 쿼터니언 회전을 위한 Yaw, Pitch, Roll 선언
		D3DXQUATERNION quatYaw, quatPitch, quatRoll;

		// 마우스 움직임을 위한 변수 선언 및 가져오기
		_long dwMouseMove = 0;
		m_dwMouseMove[ROT_X] = Engine::Get_DIMouseMove(DIMS_X);
		m_dwMouseMove[ROT_Y] = Engine::Get_DIMouseMove(DIMS_Y);


		// 카메라의 Right벡터 구하기
		vLook = pPlayerTransCom->Get_Pos() - m_vEye;      // m_vLook = 목표물 - 카메라
		//m_vLook와 월드의 up벡터(0,1,0)를 외적하여 Right벡터를 구한다.
		D3DXVec3Cross(&m_vRight, &m_vUp, &vLook);

		// 앵글값 제한을 위한 최소, 최대 값 설정
		m_fMaxAngleY = 150.f;
		m_fMinAngleY = 90.f;

		// 카메라 위치는 플레이어 Pos + 떨어질 거리
		m_vEye = pPlayerTransCom->Get_Pos() + m_vOffset;

		// 바라보는 대상은 플레이어
		m_vAt = pPlayerTransCom->Get_Pos();

		// 수평 회전 (Y축 회전)
		if (dwMouseMoveX != 0)
		{
			// 쿼터니언 회전 적용
			D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));
			// 쿼터니언 곱 해주기
			m_quaternion *= quatYaw;
			// 쿼터니언 정규화
			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		}

		// 수직 회전 (X축 회전)
		if (dwMouseMoveY != 0)
		{
			// 쿼터니언 회전 적용
			D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));
			// 쿼터니언 곱 해주기
			m_quaternion *= quatPitch;
			// 쿼터니언 정규화
			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		}

		// 현재 마우스 입력 값을 저장하여 다음 프레임에 사용
		m_dwPrevMouseMoveX = dwMouseMoveX;
		m_dwPrevMouseMoveY = dwMouseMoveY;

		// 현재 마우스 무브로 회전해서 사용안함
#pragma region 우클릭 회전
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

		//	// 가로 회전 (Y축 회전)
		//	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
		//	{
		//		D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(m_dwMouseMove[ROT_X] * m_fRotSpeed));
		//		m_quaternion *= quatYaw;
		//		D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		//	}
		//}
#pragma endregion

		_matrix matRotation; // 회전 행렬을 만들기 위한 행렬
		D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion); // 쿼터니언을 회전 행렬로 변환

		_vec3 vPos = m_vEye - pPlayerTransCom->Get_Pos();
		// 결과 값을 저장할 벡터 주소, 행렬과 곱하기를 할 위치 벡터 주소, 행렬 주소;
		D3DXVec3TransformCoord(&vPos, &vPos, &matRotation);      // 위치벡터 

		m_vEye = vPos + pPlayerTransCom->Get_Pos();

	}
}

void CDynamicCamera::Update_BlackBoard()
{
	// 블랙보드 연결 대기, 안전 코드로 필수
	if (!m_wpBlackBoard_Camera.Get_BlackBoard())
	{
		m_wpBlackBoard_Camera.Set_BlackBoard(Engine::Get_BlackBoard(L"Camera"));
		// 연결 실패
		if (!m_wpBlackBoard_Camera.Get_BlackBoard())
			return;
	}

	// 안전 코드를 거치면 일반 포인터로 접근 허용.
	CBlackBoard_Camera* pBlackBoard = m_wpBlackBoard_Camera.Get_BlackBoard();

	// 여기서부터 블랙보드의 정보를 업데이트 한다.
	pBlackBoard->Get_OneSwitch() = m_bOne;
	pBlackBoard->Get_ThreeSwitch() = m_bThree;
	pBlackBoard->Set_Eye(m_vEye);
	pBlackBoard->Set_At(m_vAt);

}

// 블랙보드 정보 내려받기 (정보 내려받기)
void CDynamicCamera::Update_InternalData()
{
	// 블랙보드 연결 대기, 안전 코드로 필수
	if (!m_wpBlackBoard_Player.Get_BlackBoard())
	{
		m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
		// 연결 실패
		if (!m_wpBlackBoard_Player.Get_BlackBoard())
			return;
	}

	// 안전 코드를 거치면 일반 포인터로 접근 허용.
	CBlackBoard_Player* m_pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

	// 여기서부터 블랙보드의 정보를 얻어온다.
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

	// 카메라 위치 설정
	m_vEye = vPosEye; // 플레이어의 눈높이

	switch (m_eRightHand_State)
	{
	case Engine::STATE_RIGHTHAND::NONE:
	{

		break;
	}
	case Engine::STATE_RIGHTHAND::HAND: // 주먹
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

					// 카메라의 up 벡터(y축) 기준으로 회전
					//D3DXMatrixRotationAxis(&matRot, &m_vUp, D3DXToRadian(m_vRotPlus.x));

					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotPlus.x));
					// 회전 결과 값과 플레이어의 Look벡터로 방향벡터 생성
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// 최종으로 구한 방향벡터를 적용
					m_vAt = vPosEye + vLook;// +vEventCameraMove;
					cout << vPosEye.x << endl;
					cout << vPosEye.y << endl;
					cout << vPosEye.z << endl;
					cout << "갱신" << endl;
				}
				else if (m_vRotPlus.x > fMoveRange && !m_bRotChange)
				{
					//m_vRot.y = 0.f;
					m_bRotChange = true;
				}

				if (m_bRotChange && m_vRotMinus.x > -fMoveRange)
				{
					m_vRotMinus.x -= fMoveSpeed * fTimeDelta;

					// 카메라의 up 벡터(y축) 기준으로 회전
					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotMinus.x));
					// 회전 결과 값과 플레이어의 Look벡터로 방향벡터 생성
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// 최종으로 구한 방향벡터를 적용
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

		// 오른손 주먹
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

					// 카메라의 up 벡터(y축) 기준으로 회전
					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotMinus.x));
					// 회전 결과 값과 플레이어의 Look벡터로 방향벡터 생성
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// 최종으로 구한 방향벡터를 적용
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

					// 카메라의 up 벡터(y축) 기준으로 회전
					D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_vRotPlus.x));
					// 회전 결과 값과 플레이어의 Look벡터로 방향벡터 생성
					D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
					// 최종으로 구한 방향벡터를 적용
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

	//// 카메라 위치 설정
	//m_vEye = vPlayerPosEye; // 플레이어의 눈높이

	//// 카메라가 바라보는 대상 (플레이어가 바라보는 방향)
	//m_vAt = vPlayerPosEye + vPlayerLook;// +vEventCameraMove;

	//Mouse_Move(); // 마우스 이동
	//Mouse_Fix();  // 마우스 고정
}

