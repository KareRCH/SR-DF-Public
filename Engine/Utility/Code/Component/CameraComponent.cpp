#include "CameraComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

CCameraComponent::CCameraComponent()
{
}

CCameraComponent::CCameraComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev), m_hWnd(HWND())
	, m_vPos(_vec3({ 0.f })), m_vLook(_vec3({ 0.f })), m_vUp(_vec3({ 0.f })), m_vLook_PreFrame(_vec3({0.f}))
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProjection);
}

CCameraComponent::CCameraComponent(const CCameraComponent& rhs)
	: Base(rhs), m_hWnd(rhs.m_hWnd)
	, m_vPos(rhs.m_vPos), m_vLook(rhs.m_vLook), m_vUp(rhs.m_vUp), m_vLook_PreFrame(rhs.m_vLook_PreFrame)
	, m_matView(rhs.m_matView), m_matProjection(rhs.m_matProjection)
{

}

CCameraComponent::~CCameraComponent()
{
}

HRESULT CCameraComponent::Ready_Camera(HWND hWnd)
{
	m_vPos = { 100.f, 10.f, -10.f };
	m_vLook = { 100.f, 10.f, 0.f };
	m_vUp = { 0.f, 1.f, 0.f };
	// 행렬에 카메라 요소를 적용
	D3DXMatrixLookAtLH(
		&m_matView,
		&m_vPos,
		&m_vLook,
		&m_vUp);
	// 실제 장치에 카메라 행렬을 적용
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// 투영 행렬을 설정한다.
	D3DXMatrixPerspectiveFovLH(
		&m_matProjection,
		D3DX_PI * 0.5f,
		(float)WINCX / (float)WINCY,
		1.f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProjection);

	m_hWnd = hWnd;

	RECT rcWindow = {};
	GetWindowRect(m_hWnd, &rcWindow);
	RECT rcClient = {};
	GetClientRect(m_hWnd, &rcClient);
	POINT ptScreen = { rcWindow.left + ((rcClient.right - rcClient.left) / 2), rcWindow.top + ((rcClient.bottom - rcClient.top) / 2) };
	SetCursorPos(ptScreen.x, ptScreen.y);

	return S_OK;
}

_int CCameraComponent::Update_Component(const _float& fTimeDelta)
{
	// 발작버튼
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_vPos.x = (_float)(rand() % 100);
		m_vPos.y = (_float)(rand() % 100);
		m_vPos.y = (_float)(rand() % 100);
	}

	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProjection);

	// 마우스의 위치를 얻는다.
	RECT rcClient = {};
	POINT ptMouse = Get_MousePos_Client(m_hWnd);
	GetClientRect(m_hWnd, &rcClient);

	// 오너의 바라보는 방향을 얻는다.
	NULL_CHECK_RETURN(m_pOwner, E_FAIL);
	
	/*CTransformComponent* pTransform = dynamic_cast<CTransformComponent*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	NULL_CHECK_RETURN(pTransform, E_FAIL);

	_vec3 vOwnerLook;
	pTransform->Get_Info(INFO_LOOK, &vOwnerLook);*/

	// 화면의 중앙을 향하는 벡터를 생성한다.
	_vec3 vLookMouse = { _float(ptMouse.x - (rcClient.right / 2)), -_float(ptMouse.y - (rcClient.bottom / 2)), 0.f};

	// 마우스가 움직인대로 Axis 변경을 통해 
	//D3DXMatrixRotationAxis(pTransform->Get_WorldMatrix(), &_vec3(0.f, 1.f, 0.f), m_vLook_PreFrame);
	m_vLook.x += (vLookMouse.x - m_vLook_PreFrame.x) * fTimeDelta;
	m_vLook.y += (vLookMouse.y - m_vLook_PreFrame.y) * fTimeDelta;
	//D3DXMatrixRotationY(pTransform->Get_WorldMatrix(), (vLookMouse.x - m_vLook_PreFrame.x));
	//D3DXMatrixRotationX(pTransform->Get_WorldMatrix(), (vLookMouse.y - m_vLook_PreFrame.y));

	// 이전 프레임의 Look 방향 벡터를 저장한다.
	m_vLook_PreFrame = vLookMouse;

	// 마우스 좌표를 다시 화면에 고정시킨다.
	RECT rcWindow = {};
	GetWindowRect(m_hWnd, &rcWindow);
	GetClientRect(m_hWnd, &rcClient);
	POINT ptScreen = { rcWindow.left + ((rcClient.right - rcClient.left) / 2), rcWindow.top + ((rcClient.bottom - rcClient.top) / 2) };
	SetCursorPos(ptScreen.x, ptScreen.y);

	// 행렬에 카메라 요소를 적용
	D3DXMatrixLookAtLH(
		&m_matView, 
		&m_vPos, 
		&m_vLook, 
		&m_vUp);
	// 실제 장치에 카메라 행렬을 적용
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// 투영 행렬을 설정한다.
	D3DXMatrixPerspectiveFovLH(
		&m_matProjection,
		D3DX_PI * 0.5f,
		(float)WINCX / (float)WINCY,
		1.f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProjection);

	return 0;
}

void CCameraComponent::Input(const _float& fTimeDelta)
{

}

CCameraComponent* CCameraComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Camera(hWnd)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCameraComponent Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CCameraComponent::Clone()
{
	return new ThisClass(*this);
}

void CCameraComponent::Free()
{
	SUPER::Free();
}
