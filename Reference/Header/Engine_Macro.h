#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{
#define	WINCX		1280 //임시
#define	WINCY		720

#define	VTXCNTX		129
#define VTXCNTZ		129
#define VTXITV		1


// 치환 매크로
#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#define			SUPER __super

// 특수한 함수 앞에 별도로 붙이기 위한 용도
#define PRIVATE		private:	// private:
#define PROTECTED	protected:	// protected:
#define PUBLIC		public:		// public:

// 눈에 잘 띄게 매크로로 만든 예약어
#define OVERRIDE	override	// override

// 빌드 옵션 매크로
#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


// 검사 매크로
#define NULL_CHECK( _ptr)	\
	{if( (_ptr) == 0){ DebugBreak();return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( (_ptr) == 0){ DebugBreak();return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( (_ptr) == 0){MessageBox(NULL, _message, L"System Message",MB_OK); DebugBreak();}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( (_ptr) == 0){MessageBox(NULL, _message, L"System Message",MB_OK); DebugBreak();return _return;}}



#define FALSE_CHECK( _false)	\
	NULL_CHECK( _false)

#define FALSE_CHECK_RETURN( _false, _return)	\
	NULL_CHECK_RETURN( _false, _return)

#define FALSE_CHECK_MSG( _false, _message )	\
	NULL_CHECK_MSG( _false, _message )

#define FALSE_CHECK_RETURN_MSG( _false, _return, _message )	\
	NULL_CHECK_RETURN_MSG( _false, _return, _message )



#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); DebugBreak(); return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); DebugBreak(); return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); DebugBreak();}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(NULL, _message, L"System Message",MB_OK); DebugBreak();return _return;}



// 생성 규칙 매크로
#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static void DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)					\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
			if(NULL == m_pInstance) {					\
				m_pInstance = new CLASSNAME;			\
			}											\
			return m_pInstance;							\
		}												\
		void CLASSNAME::DestroyInstance( void ) {		\
			if(NULL != m_pInstance)	{					\
				delete m_pInstance;						\
				m_pInstance = NULL;						\
			}											\
		}

#define			THIS_CLASS(CLASSNAME)		using ThisClass = CLASSNAME;
#define			BASE_CLASS(CLASSNAME)		using Base = CLASSNAME;
#define			DERIVED_CLASS(BASENAME, THISCLASS)	\
					THIS_CLASS(THISCLASS)			\
					BASE_CLASS(BASENAME)
#define			DERIVED_CLASS_SINGLETON(BASENAME, THISCLASS) \
					DERIVED_CLASS(BASENAME, THISCLASS)	\
					DECLARE_SINGLETON(THISCLASS)

// getset 매크로
// 일반 Set 함수
#define SET(TYPE, MVALUE, NAME)										\
			void Set_##NAME(TYPE value) { MVALUE = value; }
// 입력 값이 const Type
#define SET_C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(const TYPE value) { MVALUE = value; }
// 입력 값이 Type const (포인터 용)
#define SET__C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE const value) { MVALUE = value; }
// 입력 값이 const Type const (포인터 용)
#define SET_C_C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(const TYPE const value) { MVALUE = value; }

// 입력 값이 Type*
#define SET_PTR(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE* value) { MVALUE = *value; }
// 입력 값이 cosnt Type*
#define SET_C_PTR(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE* value) { MVALUE = *value; }
// 입력 값이 Type const*
#define SET_PTR_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE const* value) { MVALUE = *value; }
// 입력 값이 const Type const*
#define SET_C_PTR_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE const* value) { MVALUE = *value; }

// 입력 값이 Type&
#define SET_REF(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE& value) { MVALUE = value; }
// 입력 값이 const Type&
#define SET_C_REF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE& value) { MVALUE = value; }
// 입력 값이 Type const&
#define SET_REF_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE& const value) { MVALUE = value; }
// 입력 값이 const Type const&
#define SET_C_REF_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE const& value) { MVALUE = value; }

// 입력 값이 Type&&
#define SET_RREF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE&& value) { MVALUE = value; }
// 입력 값이 const Type&&
#define SET_C_RREF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE&& value) { MVALUE = value; }


// 반환값이 Type
#define GET(TYPE, MVALUE, NAME)										\
			TYPE Get_##NAME() { return MVALUE; }	
// 반환값이 const Type
#define GET_C(TYPE, MVALUE, NAME)									\
			const TYPE Get_##NAME() const { return MVALUE; }	
// 반환값이 Type const
#define GET__C(TYPE, MVALUE, NAME)									\
			TYPE const Get_##NAME() const { return MVALUE; }	
// 반환값이 const Type const
#define GET_C_C(TYPE, MVALUE, NAME)									\
			const TYPE const Get_##NAME() const { return MVALUE; }	

// 반환값이 Type&
#define GET_REF(TYPE, MVALUE, NAME)									\
			TYPE& Get_##NAME() { return MVALUE; }	
// 반환값이 const Type&
#define GET_C_REF(TYPE, MVALUE, NAME)								\
			const TYPE& Get_##NAME() const { return MVALUE; }	
// 반환값이 Type const&
#define GET_REF_C(TYPE, MVALUE, NAME)								\
			TYPE const& Get_##NAME() const { return MVALUE; }	
// 반환값이 const Type const&
#define GET_C_REF_C(TYPE, MVALUE, NAME)								\
			const TYPE const& Get_##NAME() const { return MVALUE; }

// 반환값이 Type* 
#define GET_PTR(TYPE, MVALUE, NAME)									\
			TYPE* Get_##NAME() { return &MVALUE; }
// 반환값이 const Type*
#define GET_C_PTR(TYPE, MVALUE, NAME)								\
			const TYPE* Get_##NAME() const { return &MVALUE; }
// 반환값이 Type const*
#define GET_PTR_C(TYPE, MVALUE, NAME)								\
			TYPE const* Get_##NAME() const { return &MVALUE; }
// 반환값이 const Type const*
#define GET_C_PTR_C(TYPE, MVALUE, NAME)								\
			const TYPE const* Get_##NAME() const { return &MVALUE; }



// 복수 GET 매크로
#pragma region GET SET 함수 복수로드 매크로
#define GETSET_EX1(TYPE, MVALUE, NAME, ARG1)		\
			ARG1##(TYPE, MVALUE, NAME)

#define GETSET_EX2(TYPE, MVALUE, NAME, ARG1, ARG2)			  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)

#define GETSET_EX3(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3)			  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)

#define GETSET_EX4(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4)				  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)

#define GETSET_EX5(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5)				  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)

#define GETSET_EX6(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)					  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)

#define GETSET_EX7(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)					 \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)

#define GETSET_EX8(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)					 \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)	\
			ARG8##(TYPE, MVALUE, NAME)

#define GETSET_EX9(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)			   \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)	\
			ARG8##(TYPE, MVALUE, NAME)	\
			ARG9##(TYPE, MVALUE, NAME)

#define GETSET_EX10(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9, ARG10)		\
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)	\
			ARG8##(TYPE, MVALUE, NAME)	\
			ARG9##(TYPE, MVALUE, NAME)	\
			ARG10##(TYPE, MVALUE, NAME)
#pragma endregion


#define DXCOLOR_WHITE         D3DCOLOR_ARGB(255, 255, 255, 255) // 흰색
#define DXCOLOR_BLACK         D3DCOLOR_ARGB(255,   0,   0,   0) // 검은색
#define DXCOLOR_RED           D3DCOLOR_ARGB(255, 255,   0,   0) // 빨간색
#define DXCOLOR_PURPLE        D3DCOLOR_ARGB(255, 128,   0, 128) // 자주색
#define DXCOLOR_MAGENTA       D3DCOLOR_ARGB(255, 255,   0, 255) // 심홍색
#define DXCOLOR_ORANGE        D3DCOLOR_ARGB(255, 255, 165,   0) // 주황색
#define DXCOLOR_DARK_ORANGE   D3DCOLOR_ARGB(255, 115,  57,   0) // 짙은 주황색
#define DXCOLOR_YELLOW        D3DCOLOR_ARGB(255, 255, 255,   0) // 노란색
#define DXCOLOR_GREEN         D3DCOLOR_ARGB(255,   0, 255,   0) // 초록색
#define DXCOLOR_CYAN          D3DCOLOR_ARGB(255,   0, 255, 255) // 청록색
#define DXCOLOR_DARK_GREEN    D3DCOLOR_ARGB(255,   0, 128,   0) // 어두운 초록색
#define DXCOLOR_OLIVE         D3DCOLOR_ARGB(255, 128, 128,   0) // 올리브색
#define DXCOLOR_BLUE          D3DCOLOR_ARGB(255,   0,   0, 255) // 파란색
#define DXCOLOR_NAVY          D3DCOLOR_ARGB(255,   0,   0, 128) // 짙은 파란색
#define DXCOLOR_TEAL          D3DCOLOR_ARGB(255,   0, 128, 128) // 옥색
#define DXCOLOR_MAROON        D3DCOLOR_ARGB(255, 128,   0,   0) // 고동색
#define DXCOLOR_PINK          D3DCOLOR_ARGB(255, 255, 192, 203) // 분홍색
#define DXCOLOR_LAVENDER      D3DCOLOR_ARGB(255, 230, 230, 250) // 라벤더색
#define DXCOLOR_GOLD          D3DCOLOR_ARGB(255, 255, 215,   0) // 금색
#define DXCOLOR_SILVER        D3DCOLOR_ARGB(255, 192, 192, 192) // 은색
#define DXCOLOR_BROWN         D3DCOLOR_ARGB(255, 165,  42,  42) // 갈색
#define DXCOLOR_GRAY          D3DCOLOR_ARGB(255, 128, 128, 128) // 회색
#define DXCOLOR_LIGHT_GRAY    D3DCOLOR_ARGB(255, 194, 194, 194) // 회색
#define DXCOLOR_INDIGO        D3DCOLOR_ARGB(255,  75,   0, 130) // 남색
#define DXCOLOR_TURQUOISE     D3DCOLOR_ARGB(255,  64, 224, 208) // 터키스색
#define DXCOLOR_LIME          D3DCOLOR_ARGB(255,   0, 255,   0) // 라임색

}


			

#endif // Engine_Macro_h__
