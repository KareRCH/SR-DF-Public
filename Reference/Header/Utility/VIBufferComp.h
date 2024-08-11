#pragma once

#include "SceneComponent.h"

BEGIN(Engine)

/// <summary>
/// ���� ���� ���۸� �����ϴ� Ŭ����
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

	_ulong			m_dwVtxCnt;			// ������ ����
	_ulong			m_dwVtxSize;		// ������ ũ��
	_ulong			m_dwTriCnt;			// �ﰢ���� ����
	_ulong			m_dwFVF;			// ������ �Ӽ�

	_ulong			m_dwIdxSize;		// �ε��� ������
	D3DFORMAT		m_IdxFmt;			// �ε��� ����

public:
	virtual void Free();
};

END