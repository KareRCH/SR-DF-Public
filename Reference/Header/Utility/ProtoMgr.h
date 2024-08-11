#pragma once

#include "Engine_Define.h"
#include "Base.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CProtoMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CProtoMgr)

private:
	explicit CProtoMgr();
	virtual ~CProtoMgr();

public:
	HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent);
	CComponent*		Clone_Proto(const _tchar* pProtoTag);
	CComponent*		Clone_Proto(const _tchar* pProtoTag, CComponent*& prComponent);

private:
	CComponent*		Find_Prototype(const _tchar* pProtoTag);

private:
	_unmap<wstring, CComponent*>		m_mapProto;

public:
	virtual void	Free();
};

END