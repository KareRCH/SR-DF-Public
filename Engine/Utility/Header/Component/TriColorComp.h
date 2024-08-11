#pragma once

#include "Engine_Define.h"
#include "VIBufferComp.h"

BEGIN(Engine)

/// <summary>
/// �ﰢ���� ���� �Բ� �׸��� ������Ʈ
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
	/// CTriColorComp ���丮 �Լ�
	/// </summary>
	/// <param name="pGraphicDev">���̷�Ʈ ��ġ</param>
	/// <returns>���� �ν��Ͻ�</returns>
	static CTriColorComp*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	/// <summary>
	/// CTriColorComp ������Ÿ�� �Լ�
	/// </summary>
	/// <returns>����</returns>
	virtual	CComponent* Clone();

private:
	/// <summary>
	/// ��ӵ� ����
	/// </summary>
	virtual void		Free();

	// CVIBufferComp��(��) ���� ��ӵ�
	virtual HRESULT Ready_Component() override;
	virtual void LateUpdate_Component() override;
	virtual void Render_Component() override;
};

END