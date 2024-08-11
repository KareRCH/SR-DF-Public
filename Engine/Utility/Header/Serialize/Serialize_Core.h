#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

enum ESERIALIZE_PROCESS : _uint
{
	ESERIALIZE_PROCESS_IMMEDIATE,
	ESERIALIZE_PROCESS_INIT,
	ESERIALIZE_PROCESS_UPDATE,
	ESERIALIZE_PROCESS_END
};

enum ESERIALIZE_TYPE
{
	ESERIALIZE_PROJECT,
	ESERIALIZE_SCENE,
	ESERIALIZE_LAYER,
	ESERIALIZE_OBJECT,
	ESERIALIZE_PROTO,
	ESERIALIZE_TERRAIN,
	ESERIALIZE_COMPONENT,
	ESERIALIZE_LIGHT,
	ESERIALIZE_SIZE
};

/// <summary>
/// ��� ����ȭ �����Ͱ� ���������� ������ ��� ������
/// </summary>
class ENGINE_DLL FSeialize_Header
{
public:
	ESERIALIZE_TYPE		eType = static_cast<ESERIALIZE_TYPE>(0);		// Ÿ�� �±�
	string				strName = "";		// �̸� �±�
};

END