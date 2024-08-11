#pragma once

#include "Engine_Define.h"
#include "VIBufferComp.h"

BEGIN(Engine)

/// <summary>
/// 삼각형을 색과 함께 그리는 컴포넌트
/// </summary>
class ENGINE_DLL CTriColorComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CTriColorComp)
private:
	explicit CTriColorComp();
	explicit CTriColorComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTriColorComp(const CTriColorComp& rhs);
	virtual ~CTriColorComp();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	/// <summary>
	/// CTriColorComp 팩토리 함수
	/// </summary>
	/// <param name="pGraphicDev">다이렉트 장치</param>
	/// <returns>생성 인스턴스</returns>
	static CTriColorComp*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	/// <summary>
	/// CTriColorComp 프로토타입 함수
	/// </summary>
	/// <returns>복사</returns>
	virtual	CComponent* Clone();

private:
	/// <summary>
	/// 상속된 해제
	/// </summary>
	virtual void		Free();

	// CVIBufferComp을(를) 통해 상속됨
	virtual HRESULT Ready_Component() override;
	virtual void LateUpdate_Component() override;
	virtual void Render_Component() override;
};

END