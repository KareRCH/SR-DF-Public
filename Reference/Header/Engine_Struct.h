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

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // �ؽ�ó�� UV ��ǥ ���� FLOAT�� 3���� ǥ���ϰڴٴ� ��ũ��(��ȣ���� ���� 0�� �ǹ̴� ���� ���ؽ��� �ؽ��� UV���� �������� �� �� �ִµ� ���� 0��° ���� �����ϰڴٴ� �ǹ�)

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
		tagAttribute() // ������ 
		{
			fLifeTime = 0.0f;
			fAge = 0.0f;
			bIsAlive = true;
		}
		_vec3		vPosition; // ���� ��ġ�� ����
		_vec3		vVelocity; // ���� �ӵ��� ����
		_vec3		vAcceleration; // ���� ���ӵ��� ���� 
		_float      fLifeTime;     // ��ü�� ����ִµ����� ���� - �󸶳� ���� �����Ұ��ΰ� 
		_float      fAge;          // ��ü�� ���� ���� - �󸶳� ���� �����ߴ°� 
		D3DXCOLOR   Color;        // ���� ���� - � �������� ������ �Ǿ�� �ϴ°� 
		D3DXCOLOR   ColorFade;    // �ð��� ���� ������ ��� ���ؾ� �ϴ°�
		_bool       bIsAlive; // ��ü�� ����ִ��� ���� - Ȱ���������� �ƴϸ� �Ҹ��ߴ��� 

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


#pragma region ���� ���¸ӽ�
	// ���� �ƹ����� �� �� �ִ� ���¸ӽ� ����ü
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

		bool	bIsEnter;	// ����
		bool	bIsExit;	// Ż��
		bool	bIsReserved;// ����
		T		eState;		// ���� ����
		T		ePrevState; // ���� ����
		T		eNextState; // ���� ����

#pragma region ���¸ӽ� �Լ�
		
		// ���� ���¸� �ٲٴ� �Լ� (Ż�� �ڵ�� ���� �ڵ带 �۵� ��Ų��.)
		void Set_State(T _eState)
		{
			ePrevState = eState;
			eState = _eState;
			bIsExit = true;
			bIsEnter = true;
		}

		// ���� ��ȯ ���� �Լ�
		void Reserve_State(T _eState)
		{
			eNextState = _eState;
			bIsReserved = true;
		}

		// ������ ��
		bool IsState_Entered()
		{
			if (bIsEnter && !bIsReserved)
			{
				bIsEnter = false;
				bIsExit = false;		// ���� ���Խ� Ż�� ���� �ڵ� ��Ȱ��ȭ
				return true;
			}
			return false;
		}

		// �������� ��
		bool IsState_Exit()
		{
			// ���� ���¿����� ���� ���¸� Set_State �Ѵ�.
			if (bIsReserved)
			{
				Set_State(eNextState);
				bIsReserved = false;
				bIsExit = false;
				return true;
			}

			// �̹� Entered �Լ��� �ҷ����� �� Ż�� ������ OFF ��Ų��.
			if (!bIsEnter)
				bIsExit = false;

			if (bIsExit)
			{
				bIsExit = false;
				return true;
			}
			return false;
		}

		// ������ ���� ���¿����� ������Ʈ�� �� �� �ִ�.
		bool Can_Update()
		{
			return !bIsReserved;
		}

		// ���� ���°� �Ű������� �´��� üũ
		bool IsOnState(T _eState)
		{
			return (eState == _eState);
		}
#pragma endregion
	};

	// ���ǻ��� : Ű���� ���ڿ��� �ҰŸ� ��Ʈ������ ����ؾ� �Ѵ�.

	template <typename T>
	using STATE_INFO = tagState<T>;

	// �ܼ��ϰ� ���� ���� ���ø�
	template <typename Key, typename Func>
	using MAP_FUNC = unordered_map<Key, function<Func>>;

	// ���¸ӽ� ��Ʈ
	template<typename Key, typename Func>
	struct STATE_SET
	{
		STATE_SET() {}
		~STATE_SET() {}

	public:
		STATE_INFO<Key> tState;
		MAP_FUNC<Key, Func> mapFunc;

	public:
#pragma region ���¸ӽ� �Լ�
		void Set_State(Key _eState)
		{
			tState.Set_State(_eState);
		}

		// + ���� �߰� : ���� ���°��� ��ȯ
		Key Get_State()
		{
			return tState.eState;
		}

		void Reserve_State(Key _eState)
		{
			tState.Reserve_State();
		}

		// ������ ��
		bool IsState_Entered()
		{
			return tState.IsState_Entered();
		}

		// �������� ��
		bool IsState_Exit()
		{
			return tState.IsState_Exit();
		}

		// ������ ���� ���¿����� ������Ʈ�� �� �� �ִ�.
		bool Can_Update()
		{
			return tState.Can_Update();
		}

		bool IsOnState(Key _eState)
		{
			return tState.IsOnState(_eState);
		}
#pragma endregion
#pragma region �Լ� ��
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

#pragma region ������
	// ������ �뵵�� ���� ����ü
	template<typename T = float>
	struct _DELAY
	{
		static_assert(std::is_arithmetic<T>::value, "T�� ���� Ÿ���̾�߸� �մϴ�.");

	public:
		T Max, Cur;
	private:
		T PrevCur;


	public:
		_DELAY() : Max(T()), Cur(T()), PrevCur(Cur) {}
		_DELAY(T _Max, bool bMax = false) : Max(_Max), Cur(T(T(bMax)* T(_Max))), PrevCur(Cur) {}
		~_DELAY() {}

		// �� ������Ʈ �� �ƽ��� ���޽� ��ȯ
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

		// Ư�� ����Ʈ(����)�� �Ѿ�� ��ȯ
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

		// ���簪 �ʱ�ȭ
		void Reset()
		{
			Cur = T();
		}

		// Max �� �缳�� �� ���簪 �ʱ�ȭ
		void ReAdjust(T max)
		{
			Max = max;
			Cur = T();
		}

		// Ư�� ����Ʈ(����)�� �Ѿ�� ��� Ʈ��
		bool IsReach(T point)
		{
			return (Cur >= point);
		}

		// �������� üũ�ϰ��� �ϴ� ������ �ѹ��� ������ ���� üũ�մϴ�.
		bool IsReach_Once(T point, T increase)
		{
			return (Cur >= point - increase * (T)0.5f && Cur < point + increase * (T)0.5f);
		}

		// �ƽ��� ��� ��� Ʈ��
		bool IsMax()
		{
			return (Cur >= Max);
		}

		// �ƽ��� ��� �ѹ��� Ʈ��
		bool IsMax_Once()
		{
			return (Cur >= Max && PrevCur != Cur);
		}

		// �ۼ�Ʈ �� ��ȯ
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

#pragma region �׼�
	typedef struct _ACTION
	{
		_ACTION() : bAction() {}
		~_ACTION() {}

		bool bAction;

		/// <summary>
		/// �� ������ �������� ���� ��
		/// </summary>
		void Act()
		{
			bAction = true;
		}

		/// <summary>
		/// �� ������ �������� ���� ��
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

	// ���� �߰�
	enum class OBJ_TYPE // ������Ʈ�� Ÿ��
	{
		NONE,
		TWO_HAND,
		TWO_OBJECT,
		RIGHT_OBJECT
	};

	enum class OBJ_NAME // ������Ʈ�� �̸�
	{
		NONE,
		GUN,
		THOMPSON,
		STEELPIPE,
		BEERBOTLE,
		FRYINGPAN
	};

	// ���� �߰�
#pragma region Ű������(�ִϸ��̼�) ����ü

	// Ű������ ����ü
	struct KEYFRAME
	{
		float time = 0.f;					// Ű�������� �ð� (0.0f ~ MaxTime ����)
		float value = 0.f;					// �ִϸ��̼� �� (ũ��, ȸ��, �̵� ��)

		int type = 0;					// �ִϸ��̼� Ÿ�� (0: ũ��, 1: ȸ��, 2: �̵�)

		bool isEaseIn = false;				// Ease In ���� (True �Ǵ� False)
		bool isEaseOut = false;				// Ease Out ���� (True �Ǵ� False)

		bool bChargePossible = false;		// ���� ���� ����
		bool bShieldPossible = false;		// ��� ���� ����

		bool bFullChargeKeyframe = false;	// Ǯ������ ������
		bool bShieldKeyFrame = false;		// ����� ������

		int texureframe = 0;				// �ؽ�ó ���� ��

		_vec3	vScale = { 0.f, 0.f, 0.f };			// ũ�⸦ ���� �׸�
		_vec3	vRot = { 0.f, 0.f, 0.f };			// ȸ���� ���� �׸�
		_vec3	vPos = { 0.f, 0.f, 0.f };			// ��ġ�� ���� �׸�

		_vec2	vKeyFramePos = { 0.00000000f, 0.00000000f };		// �������� �ش� Ű������ ��ġ

		//OBJ_TYPE m_eObjectType = OBJ_TYPE::NONE; // Ÿ���� �ο��� �׸� (ex : �Ѽ�, ���)
		//OBJ_NAME m_eObjectName = OBJ_NAME::NONE; // �̸��� �ο��� �׸� (ex : ����, ��������)

	};

#pragma endregion

#pragma endregion
}


#endif // Engine_Struct_h__
