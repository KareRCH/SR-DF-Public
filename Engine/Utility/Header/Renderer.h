#pragma once

#include "GameObject.h"

BEGIN(Engine)

// 렌더타겟용 뷰포트 열거체
enum class EVIEWPORT : _uint
{
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, SIZE
};

// 렌더타겟용 뷰포트 열거체
enum class EVIEWPORT_RT : _uint
{
	NORMAL, DEPTH, HEIGHT, SIZE
};

/// <summary>
/// 렌더러는 기존 레이어의 Rendering 역할을 부여받은 클래스로
/// 렌더링 목적에 따라 처리 함수를 달리하여 수행한다.
/// 매 프레임마다 렌더그룹에 추가해주어야 합니다.
/// </summary>
class ENGINE_DLL CRenderer : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

private:
	virtual void	Free();

public:
	HRESULT			Ready_Renderer(const _uint iWidth = 1200U, const _uint iHeight = 900U);
	void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Clear_RenderGroup();

public:
	GETSET_EX1(HRESULT, m_hReadyResult, ReadyResult, GET_C_REF)

private:
	HRESULT			m_hReadyResult;


public:
	void			Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_AlphaTest(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	list<CGameObject*>		m_RenderGroup[RENDER_END];


public:
#pragma region 트랜스폼 변경 함수들
	// 원근 뷰 행렬을 계산한다.
	inline void Readjust_PersView();
	// 직교 뷰 행렬을 계산한다.
	inline void Readjust_OrthoView();
	
#pragma endregion

public:		// 트랜스폼 영역, Transform에서 옮겨온 거임
	GETSET_EX1(_vec3, m_vInfo[INFO_RIGHT],	Right,	GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_UP],		Up,		GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_LOOK],	Look,	GET_C_REF)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS],	Pos,	GET_C_REF, SET_C)
	void Set_PosX(const _float value) { m_vInfo[INFO_POS].x = value; }
	void Set_PosY(const _float value) { m_vInfo[INFO_POS].y = value; }
	void Set_PosZ(const _float value) { m_vInfo[INFO_POS].z = value; }

	GETSET_EX2(_vec3,	m_vRotation,		Rotation,		GET_C_REF, SET_C)
	void Set_RotationX(const _float value) { m_vRotation.x = value; }
	void Set_RotationY(const _float value) { m_vRotation.y = value; }
	void Set_RotationZ(const _float value) { m_vRotation.z = value; }

	GETSET_EX2(_vec3,	m_vScale,			Scale,			GET_C_REF, SET_C)
	void Set_ScaleX(const _float value) { m_vScale.x = value; }
	void Set_ScaleY(const _float value) { m_vScale.y = value; }
	void Set_ScaleZ(const _float value) { m_vScale.z = value; }

	GETSET_EX2(_matrix,					m_matPersView,		MatPersView,				GET_PTR, SET_C_PTR)
	GETSET_EX2(_matrix,					m_matOrthoView,		MatOrthoView,				GET_PTR, SET_C_PTR)
	GETSET_EX2(_matrix,					m_matOrthoProject,	MatOrthoProject,			GET_PTR, SET_C_PTR)


private:	// 렌더러의 위치 속성은 뷰를 기반으로 
	_vec3		m_vInfo[INFO_END];		// 위치, 방향 정보
	_vec3		m_vRotation;			// 오일러 회전축
	_vec3		m_vScale;				// 크기
	_matrix		m_matPersView;			// 원근용 뷰 행렬
	_matrix		m_matOrthoView;			// 직교용 뷰 행렬
	_matrix		m_matOrthoProject;		// UI용 직교투영 범위

public:
	GETSET_EX1(vector<D3DVIEWPORT9>,	m_vecViewport,		VecViewport,				GET_REF)
	D3DVIEWPORT9&	Get_Viewport(_uint value) { return m_vecViewport[value]; }

	GETSET_EX1(vector<D3DVIEWPORT9>,	m_vecViewport_RT,	VecViewport_RenderTarget,	GET_REF)

private:

	vector<D3DVIEWPORT9>		m_vecViewport;			// 일반 뷰포트 세팅
	vector<D3DVIEWPORT9>		m_vecViewport_RT;		// 렌더 타겟 뷰포트

};

END



inline void CRenderer::Readjust_PersView()
{
	D3DXMatrixIdentity(&m_matPersView);

	// 3x3만큼 월드 행렬로부터 vInfo에 복사
	for (_int i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matPersView.m[i][0], sizeof(_vec3));

	// 크기 변환
	for (_int i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	// 회전 변환
	_matrix		matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], m_vRotation.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vRotation.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vRotation.z);

	for (_int i = 0; i < INFO_POS; ++i)
	{
		for (_int j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	// 월드 행렬 구성
	for (_int i = 0; i < INFO_END; ++i)
		memcpy(&m_matPersView.m[i][0], &m_vInfo[i], sizeof(_vec3));
}

inline void CRenderer::Readjust_OrthoView()
{
	D3DXMatrixIdentity(&m_matOrthoView);

	// 3x3만큼 월드 행렬로부터 vInfo에 복사
	for (_int i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matOrthoView.m[i][0], sizeof(_vec3));

	// 크기 변환
	for (_int i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	// 회전 변환
	_matrix		matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], m_vRotation.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vRotation.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vRotation.z);

	for (_int i = 0; i < INFO_POS; ++i)
	{
		for (_int j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	// 월드 행렬 구성
	for (_int i = 0; i < INFO_END; ++i)
		memcpy(&m_matOrthoView.m[i][0], &m_vInfo[i], sizeof(_vec3));
}