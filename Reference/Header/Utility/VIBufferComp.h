#pragma once

#include "SceneComponent.h"

BEGIN(Engine)

/// <summary>
/// 점에 대한 버퍼를 저장하는 클래스
/// </summary>
class ENGINE_DLL CVIBufferComp : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CVIBufferComp)
protected:
	explicit CVIBufferComp();
	explicit CVIBufferComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBufferComp(const CVIBufferComp& rhs);
	virtual ~CVIBufferComp();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	const _ulong& Get_VertexCount() { return m_dwVtxCnt; }

public:
	GETSET_EX1(LPDIRECT3DVERTEXBUFFER9, m_pVB, VertexBuffer, GET_C_REF)
	GETSET_EX1(LPDIRECT3DINDEXBUFFER9, m_pIB, IndexBuffer, GET_C_REF)

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

	_ulong			m_dwVtxCnt;			// 정점의 개수
	_ulong			m_dwVtxSize;		// 정점의 크기
	_ulong			m_dwTriCnt;			// 삼각형의 개수
	_ulong			m_dwFVF;			// 정점의 속성

	_ulong			m_dwIdxSize;		// 인덱스 사이즈
	D3DFORMAT		m_IdxFmt;			// 인덱스 포맷

public:
	virtual void Free();
};

END