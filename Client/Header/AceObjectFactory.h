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
	Create설명 

	OBJECT_CLASS objClass : enum에 따라 다른클래스 Create돌아가게 하기위해 

	const _tchar* pObjTag : L"apple"하면 사과 생성됨 

	const _float iCnt : 여기서 iCnt를 5로 쓰고있는건 프레임값을 의미함. 
						Generic Bottle이 <0u, 5u> 로 이미지를 넣었는데, 들어가는 이미지가 달라서임 (하나가 점점 움직이는것같은 프레임구성이 아니란거) 
						그냥 저기 Cnt 1~5까지 넣고 stage에 create해보면 다 다른병이 생성된거 알수있음. 그냥 bottle1 bottle2 bottle3인데 네임태그라고 보면됨 
						다른곳은 안쓰고 Throw쪽에 오브젝트가 너무많아서 그냥 이렇게 구성함. 다른클래스는 default 0으로 넣으면됨 

	ETEAM_ID _eTeamID : 생성시 디폴트로 object팀으로 배속 

	CAceObjectFactory::OBJECT_DIRECTION pDirection : 건물 돌리기용도 . 기본적으로 dds파일자체라 +x 방향으로 문이 설치되어있어서
													8방향 기준으로 건물 돌려서 셋팅가능 - 근데 무용지물임.. 알아서 툴에서 돌리더라 ㅎ
	*/
};
