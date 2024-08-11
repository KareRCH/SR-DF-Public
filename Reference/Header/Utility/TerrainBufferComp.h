#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainBufferComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CTerrainBufferComp)

protected:
	explicit CTerrainBufferComp();
	explicit CTerrainBufferComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainBufferComp(const CTerrainBufferComp& rhs);
	virtual ~CTerrainBufferComp();

#pragma region 생성, 해제
public:
	// 팩토리
	static CTerrainBufferComp* Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const _tchar* pFileName = L"",
										const _ulong& dwCntX = VTXCNTX,
										const _ulong& dwCntZ = VTXCNTZ,
										const _vec3& vScale = _vec3(1.f, 1.f, 1.f),
										const _vec3& vInvOffset = _vec3(0.f, 0.f, 0.f));

	static CTerrainBufferComp* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
										const char* pParsedFile,
										const _tchar* pFileName = L"");

	// 프로토
	virtual CComponent* Clone();

private:
	// 해제
	virtual void Free();
#pragma endregion

#pragma region 준비, 로직
public:
	virtual HRESULT		Ready_Buffer(const _tchar* pFileName, const _ulong& dwCntX, const _ulong& dwCntZ, const _vec3& vScale, const _vec3& vInvOffset);
	// 파싱 데이터 버전
	virtual HRESULT		Ready_Buffer(const char* pParsedFile, const _tchar* pHeightFile = L"");
	virtual void		Render_Buffer();
	virtual HRESULT		Ready_Component() override;
	virtual void		LateUpdate_Component() override;
	virtual void		Render_Component() override;

public:
	const _vec3* Get_VtxPos() const { return m_pPos; }
#pragma endregion

public:
	GETSET_EX2(_ulong, m_dwCountX, VertexCountX, SET, GET_C_REF)
	GETSET_EX2(_ulong, m_dwCountZ, VertexCountZ, SET, GET_C_REF)
	GETSET_EX2(_vec3, m_vInvOffset, InvOffset, SET_C, GET_C_REF)
	GETSET_EX2(_vec3, m_vScale, Scale, SET_C, GET_C_REF)
	
protected:
	_ulong			m_dwCountX;
	_ulong			m_dwCountZ;
	_vec3			m_vInvOffset;
	_vec3			m_vScale;

public:
	HRESULT			Load_HeightMap(const _tchar* pFileName = L"");

private:
	_bool			m_bHeightMap_Loaded;
	vector<_int>	m_vHeightMap;

private:
	_vec3* m_pPos;						// 버텍스 저장용 배열 포인터

private:
};

END