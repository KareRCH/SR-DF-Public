#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

using namespace std;
class unorded_map;


namespace Engine
{

	typedef struct tagVertexColor
	{
		_vec3		vPosition;		
		_ulong		dwColor;

	}VTXCOL;
	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;


	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;

	}VTXTEX;
	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;


	typedef struct tagParticle
	{
		D3DXVECTOR3 vPosition;
		D3DCOLOR    Color;
	}Particle;

	const _ulong	FVF_Particle = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	typedef struct tagAttribute
	{
		tagAttribute() // 생성자 
		{
			fLifeTime = 0.0f;
			fAge = 0.0f;
			bIsAlive = true;
		}
		_vec3		vPosition; // 현재 위치를 저장
		_vec3		vVelocity; // 현재 속도를 저장
		_vec3		vAcceleration; // 현재 가속도를 저장 
		_float      fLifeTime;     // 물체가 살아있는동안의 수명 - 얼마나 오래 존재할것인가 
		_float      fAge;          // 물체의 현재 나이 - 얼마나 오래 존재했는가 
		D3DXCOLOR   Color;        // 현재 색상 - 어떤 색상으로 렌더링 되어야 하는가 
		D3DXCOLOR   ColorFade;    // 시간에 따라 색상이 어떻게 변해야 하는가
		_bool       bIsAlive; // 물체가 살아있는지 여부 - 활성상태인지 아니면 소멸했는지 

	}Attribute;

	typedef struct Frame
	{
		//Frame
		_float		fFrame = 0.f;
		_float		fFrameEnd = 0.f;
		_float		fFrameSpeed = 0.f;

		//Repeat
		_float		fRepeat = 0;

		//LifeTime
		_float		fAge = 0.f;
		_float		fLifeTime = 0.f;

	}FRAME;

	typedef struct tagBoundingBox
	{
		_vec3 vMin;
		_vec3 vMax;
	}BoundingBox;


#pragma region 간이 상태머신
	// 대충 아무때나 쓸 수 있는 상태머신 구조체
	template <typename T>
	struct tagState
	{
		tagState() : eState(T()), ePrevState(T()), eNextState(T())
		{
			bIsEnter = false;
			bIsExit = false;
			bIsReserved = false;
		}
		~tagState() {}

		bool	bIsEnter;	// 진입
		bool	bIsExit;	// 탈출
		bool	bIsReserved;// 예약
		T		eState;		// 현재 상태
		T		ePrevState; // 이전 상태
		T		eNextState; // 다음 상태

#pragma region 상태머신 함수
		
		// 현재 상태를 바꾸는 함수 (탈출 코드랑 진입 코드를 작동 시킨다.)
		void Set_State(T _eState)
		{
			ePrevState = eState;
			eState = _eState;
			bIsExit = true;
			bIsEnter = true;
		}

		// 상태 전환 예약 함수
		void Reserve_State(T _eState)
		{
			eNextState = _eState;
			bIsReserved = true;
		}

		// 진입할 때
		bool IsState_Entered()
		{
			if (bIsEnter && !bIsReserved)
			{
				bIsEnter = false;
				bIsExit = false;		// 상태 진입시 탈출 조건 자동 비활성화
				return true;
			}
			return false;
		}

		// 빠져나갈 때
		bool IsState_Exit()
		{
			// 예약 상태에서는 다음 상태를 Set_State 한다.
			if (bIsReserved)
			{
				Set_State(eNextState);
				bIsReserved = false;
				bIsExit = false;
				return true;
			}

			// 이미 Entered 함수를 불러왔을 때 탈출 조건을 OFF 시킨다.
			if (!bIsEnter)
				bIsExit = false;

			if (bIsExit)
			{
				bIsExit = false;
				return true;
			}
			return false;
		}

		// 예약이 없는 상태에서만 업데이트를 할 수 있다.
		bool Can_Update()
		{
			return !bIsReserved;
		}

		// 현재 상태가 매개변수와 맞는지 체크
		bool IsOnState(T _eState)
		{
			return (eState == _eState);
		}
#pragma endregion
	};

	// 주의사항 : 키값을 문자열로 할거면 스트링으로 사용해야 한다.

	template <typename T>
	using STATE_INFO = tagState<T>;

	// 단순하게 쓰기 위한 템플릿
	template <typename Key, typename Func>
	using MAP_FUNC = unordered_map<Key, function<Func>>;

	// 상태머신 세트
	template<typename Key, typename Func>
	struct STATE_SET
	{
		STATE_SET() {}
		~STATE_SET() {}

	public:
		STATE_INFO<Key> tState;
		MAP_FUNC<Key, Func> mapFunc;

	public:
#pragma region 상태머신 함수
		void Set_State(Key _eState)
		{
			tState.Set_State(_eState);
		}

		// + 성희 추가 : 현재 상태값을 반환
		Key Get_State()
		{
			return tState.eState;
		}

		void Reserve_State(Key _eState)
		{
			tState.Reserve_State();
		}

		// 진입할 때
		bool IsState_Entered()
		{
			return tState.IsState_Entered();
		}

		// 빠져나갈 때
		bool IsState_Exit()
		{
			return tState.IsState_Exit();
		}

		// 예약이 없는 상태에서만 업데이트를 할 수 있다.
		bool Can_Update()
		{
			return tState.Can_Update();
		}

		bool IsOnState(Key _eState)
		{
			return tState.IsOnState(_eState);
		}
#pragma endregion
#pragma region 함수 맵
		void Add_Func(Key eState, function<Func>&& fn)
		{
			mapFunc.emplace(eState, fn);
		}

		function<Func> Get_StateFunc()
		{
			return mapFunc[tState.eState];
		}
#pragma endregion

	};
#pragma endregion

#pragma region 딜레이
	// 딜레이 용도로 만든 구조체
	template<typename T = float>
	struct _DELAY
	{
		static_assert(std::is_arithmetic<T>::value, "T는 원시 타입이어야만 합니다.");

	public:
		T Max, Cur;
	private:
		T PrevCur;


	public:
		_DELAY() : Max(T()), Cur(T()), PrevCur(Cur) {}
		_DELAY(T _Max, bool bMax = false) : Max(_Max), Cur(T(T(bMax)* T(_Max))), PrevCur(Cur) {}
		~_DELAY() {}

		// 값 업데이트 및 맥스값 도달시 반환
		bool Update(T increase, bool bAutoReset = false)
		{
			PrevCur = Cur;
			Cur += increase;
			if (Cur >= Max)
			{
				if (bAutoReset)
					Cur = T();
				else
					Cur = Max;
				return true;
			}

			return false;
		}

		// 특정 포인트(지점)를 넘어가면 반환
		bool Update(T increase, T point, bool bAutoReset = false)
		{
			PrevCur = Cur;
			Cur += increase;
			if (Cur >= point)
			{
				if (bAutoReset)
					Cur = T();
				else
					Cur = Max;
				return true;
			}

			return false;
		}

		// 현재값 초기화
		void Reset()
		{
			Cur = T();
		}

		// Max 값 재설정 및 현재값 초기화
		void ReAdjust(T max)
		{
			Max = max;
			Cur = T();
		}

		// 특정 포인트(지점)를 넘어가면 계속 트루
		bool IsReach(T point)
		{
			return (Cur >= point);
		}

		// 증가값과 체크하고자 하는 값으로 한번만 지나갈 때를 체크합니다.
		bool IsReach_Once(T point, T increase)
		{
			return (Cur >= point - increase * (T)0.5f && Cur < point + increase * (T)0.5f);
		}

		// 맥스일 경우 계속 트루
		bool IsMax()
		{
			return (Cur >= Max);
		}

		// 맥스일 경우 한번만 트루
		bool IsMax_Once()
		{
			return (Cur >= Max && PrevCur != Cur);
		}

		// 퍼센트 값 반환
		float Get_Percent()
		{
			return (static_cast<float>(Cur) / static_cast<float>(Max));
		}
	};

	template <typename T = float>
	using DELAY = _DELAY<T>;

	template <typename T = float>
	using GAUGE = _DELAY<T>;
#pragma endregion

#pragma region 액션
	typedef struct _ACTION
	{
		_ACTION() : bAction() {}
		~_ACTION() {}

		bool bAction;

		/// <summary>
		/// 값 대입을 보기좋게 만든 것
		/// </summary>
		void Act()
		{
			bAction = true;
		}

		/// <summary>
		/// 값 대입을 보기좋게 만든 것
		/// </summary>
		void Update()
		{
			bAction = false;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool Sync()
		{
			if (bAction)
			{
				bAction = false;
				return true;
			}
			return false;
		}

		bool IsOnAct()
		{
			return bAction;
		}
	}ACTION;

	template<typename Key>
	using MAP_ACTION = unordered_map<Key, ACTION>;

	template<typename Key>
	class CMapAction_Updator
	{
	public:
		void operator() (pair<const Key, ACTION>& Action)
		{
			Action.second.Update();
		}
	};

	template<typename Key>
	struct ACTION_SET
	{
		ACTION_SET() {}
		~ACTION_SET() {}

	public:
		map<Key, ACTION> mapAction;

		void Add_Action(Key&& tKey)
		{
			mapAction.emplace(tKey, ACTION());
		}

		ACTION& operator[] (Key&& tKey)
		{
			return mapAction[tKey];
		}

		void Update()
		{
			for_each(mapAction.begin(), mapAction.end(), CMapAction_Updator<Key>());
		}
	};
#pragma endregion

	// 성희 추가
	enum class OBJ_TYPE // 오브젝트의 타입
	{
		NONE,
		TWO_HAND,
		TWO_OBJECT,
		RIGHT_OBJECT
	};

	enum class OBJ_NAME // 오브젝트의 이름
	{
		NONE,
		GUN,
		THOMPSON,
		STEELPIPE,
		BEERBOTLE,
		FRYINGPAN
	};

	// 성희 추가
#pragma region 키프레임(애니메이션) 구조체

	// 키프레임 구조체
	struct KEYFRAME
	{
		float time = 0.f;					// 키프레임의 시간 (0.0f ~ MaxTime 범위)
		float value = 0.f;					// 애니메이션 값 (크기, 회전, 이동 등)

		int type = 0;					// 애니메이션 타입 (0: 크기, 1: 회전, 2: 이동)

		bool isEaseIn = false;				// Ease In 설정 (True 또는 False)
		bool isEaseOut = false;				// Ease Out 설정 (True 또는 False)

		bool bChargePossible = false;		// 차지 가능 여부
		bool bShieldPossible = false;		// 방어 가능 여부

		bool bFullChargeKeyframe = false;	// 풀차지시 프레임
		bool bShieldKeyFrame = false;		// 쉴드시 프레임

		int texureframe = 0;				// 텍스처 변경 값

		_vec3	vScale = { 0.f, 0.f, 0.f };			// 크기를 담을 그릇
		_vec3	vRot = { 0.f, 0.f, 0.f };			// 회전을 담을 그릇
		_vec3	vPos = { 0.f, 0.f, 0.f };			// 위치를 담을 그릇

		_vec2	vKeyFramePos = { 0.00000000f, 0.00000000f };		// 툴에서의 해당 키프레임 위치

		//OBJ_TYPE m_eObjectType = OBJ_TYPE::NONE; // 타입을 부여할 그릇 (ex : 한손, 양손)
		//OBJ_NAME m_eObjectName = OBJ_NAME::NONE; // 이름을 부여할 그릇 (ex : 권총, 쇠파이프)

	};

#pragma endregion

#pragma endregion
}


#endif // Engine_Struct_h__
