#pragma once
#include "GameObject.h"
#include "Engine_Enum.h"
#include "Export_System.h"
#include "Export_Utility.h"


class CAceObjectFactory
{
public:


	enum class OBJECT_DIRECTION {
		EAST, SOUTHEAST,
		SOUTH, SOUTHWEST, 
		WEST, NORTHWEST, 
		NORTH, NORTHEAST, 
		DIR_END
	};

	static Engine::CGameObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
										OBJECT_CLASS objClass,
										const _tchar* pObjTag, 
										const _float _fx, 
										const _float _fy, 
										const _float _fz,
										const _float iCnt = 0.f,
										ETEAM_ID _eTeamID = ETEAM_OBJECT,
										CAceObjectFactory::OBJECT_DIRECTION pDirection = CAceObjectFactory::OBJECT_DIRECTION::SOUTH);

	/*
	Create���� 

	OBJECT_CLASS objClass : enum�� ���� �ٸ�Ŭ���� Create���ư��� �ϱ����� 

	const _tchar* pObjTag : L"apple"�ϸ� ��� ������ 

	const _float iCnt : ���⼭ iCnt�� 5�� �����ִ°� �����Ӱ��� �ǹ���. 
						Generic Bottle�� <0u, 5u> �� �̹����� �־��µ�, ���� �̹����� �޶��� (�ϳ��� ���� �����̴°Ͱ��� �����ӱ����� �ƴ϶���) 
						�׳� ���� Cnt 1~5���� �ְ� stage�� create�غ��� �� �ٸ����� �����Ȱ� �˼�����. �׳� bottle1 bottle2 bottle3�ε� �����±׶�� ����� 
						�ٸ����� �Ⱦ��� Throw�ʿ� ������Ʈ�� �ʹ����Ƽ� �׳� �̷��� ������. �ٸ�Ŭ������ default 0���� ������� 

	ETEAM_ID _eTeamID : ������ ����Ʈ�� object������ ��� 

	CAceObjectFactory::OBJECT_DIRECTION pDirection : �ǹ� ������뵵 . �⺻������ dds������ü�� +x �������� ���� ��ġ�Ǿ��־
													8���� �������� �ǹ� ������ ���ð��� - �ٵ� ����������.. �˾Ƽ� ������ �������� ��
	*/
};
