#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{
#define	WINCX		1280 //�ӽ�
#define	WINCY		720

#define	VTXCNTX		129
#define VTXCNTZ		129
#define VTXITV		1


// ġȯ ��ũ��
#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#define			SUPER __super

// Ư���� �Լ� �տ� ������ ���̱� ���� �뵵
#define PRIVATE		private:	// private:
#define PROTECTED	protected:	// protected:
#define PUBLIC		public:		// public:

// ���� �� ��� ��ũ�η� ���� �����
#define OVERRIDE	override	// override

// ���� �ɼ� ��ũ��
#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


// �˻� ��ũ��
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



// ���� ��Ģ ��ũ��
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

// getset ��ũ��
// �Ϲ� Set �Լ�
#define SET(TYPE, MVALUE, NAME)										\
			void Set_##NAME(TYPE value) { MVALUE = value; }
// �Է� ���� const Type
#define SET_C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(const TYPE value) { MVALUE = value; }
// �Է� ���� Type const (������ ��)
#define SET__C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE const value) { MVALUE = value; }
// �Է� ���� const Type const (������ ��)
#define SET_C_C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(const TYPE const value) { MVALUE = value; }

// �Է� ���� Type*
#define SET_PTR(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE* value) { MVALUE = *value; }
// �Է� ���� cosnt Type*
#define SET_C_PTR(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE* value) { MVALUE = *value; }
// �Է� ���� Type const*
#define SET_PTR_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE const* value) { MVALUE = *value; }
// �Է� ���� const Type const*
#define SET_C_PTR_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE const* value) { MVALUE = *value; }

// �Է� ���� Type&
#define SET_REF(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE& value) { MVALUE = value; }
// �Է� ���� const Type&
#define SET_C_REF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE& value) { MVALUE = value; }
// �Է� ���� Type const&
#define SET_REF_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE& const value) { MVALUE = value; }
// �Է� ���� const Type const&
#define SET_C_REF_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE const& value) { MVALUE = value; }

// �Է� ���� Type&&
#define SET_RREF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE&& value) { MVALUE = value; }
// �Է� ���� const Type&&
#define SET_C_RREF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE&& value) { MVALUE = value; }


// ��ȯ���� Type
#define GET(TYPE, MVALUE, NAME)										\
			TYPE Get_##NAME() { return MVALUE; }	
// ��ȯ���� const Type
#define GET_C(TYPE, MVALUE, NAME)									\
			const TYPE Get_##NAME() const { return MVALUE; }	
// ��ȯ���� Type const
#define GET__C(TYPE, MVALUE, NAME)									\
			TYPE const Get_##NAME() const { return MVALUE; }	
// ��ȯ���� const Type const
#define GET_C_C(TYPE, MVALUE, NAME)									\
			const TYPE const Get_##NAME() const { return MVALUE; }	

// ��ȯ���� Type&
#define GET_REF(TYPE, MVALUE, NAME)									\
			TYPE& Get_##NAME() { return MVALUE; }	
// ��ȯ���� const Type&
#define GET_C_REF(TYPE, MVALUE, NAME)								\
			const TYPE& Get_##NAME() const { return MVALUE; }	
// ��ȯ���� Type const&
#define GET_REF_C(TYPE, MVALUE, NAME)								\
			TYPE const& Get_##NAME() const { return MVALUE; }	
// ��ȯ���� const Type const&
#define GET_C_REF_C(TYPE, MVALUE, NAME)								\
			const TYPE const& Get_##NAME() const { return MVALUE; }

// ��ȯ���� Type* 
#define GET_PTR(TYPE, MVALUE, NAME)									\
			TYPE* Get_##NAME() { return &MVALUE; }
// ��ȯ���� const Type*
#define GET_C_PTR(TYPE, MVALUE, NAME)								\
			const TYPE* Get_##NAME() const { return &MVALUE; }
// ��ȯ���� Type const*
#define GET_PTR_C(TYPE, MVALUE, NAME)								\
			TYPE const* Get_##NAME() const { return &MVALUE; }
// ��ȯ���� const Type const*
#define GET_C_PTR_C(TYPE, MVALUE, NAME)								\
			const TYPE const* Get_##NAME() const { return &MVALUE; }



// ���� GET ��ũ��
#pragma region GET SET �Լ� �����ε� ��ũ��
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


#define DXCOLOR_WHITE         D3DCOLOR_ARGB(255, 255, 255, 255) // ���
#define DXCOLOR_BLACK         D3DCOLOR_ARGB(255,   0,   0,   0) // ������
#define DXCOLOR_RED           D3DCOLOR_ARGB(255, 255,   0,   0) // ������
#define DXCOLOR_PURPLE        D3DCOLOR_ARGB(255, 128,   0, 128) // ���ֻ�
#define DXCOLOR_MAGENTA       D3DCOLOR_ARGB(255, 255,   0, 255) // ��ȫ��
#define DXCOLOR_ORANGE        D3DCOLOR_ARGB(255, 255, 165,   0) // ��Ȳ��
#define DXCOLOR_DARK_ORANGE   D3DCOLOR_ARGB(255, 115,  57,   0) // £�� ��Ȳ��
#define DXCOLOR_YELLOW        D3DCOLOR_ARGB(255, 255, 255,   0) // �����
#define DXCOLOR_GREEN         D3DCOLOR_ARGB(255,   0, 255,   0) // �ʷϻ�
#define DXCOLOR_CYAN          D3DCOLOR_ARGB(255,   0, 255, 255) // û�ϻ�
#define DXCOLOR_DARK_GREEN    D3DCOLOR_ARGB(255,   0, 128,   0) // ��ο� �ʷϻ�
#define DXCOLOR_OLIVE         D3DCOLOR_ARGB(255, 128, 128,   0) // �ø����
#define DXCOLOR_BLUE          D3DCOLOR_ARGB(255,   0,   0, 255) // �Ķ���
#define DXCOLOR_NAVY          D3DCOLOR_ARGB(255,   0,   0, 128) // £�� �Ķ���
#define DXCOLOR_TEAL          D3DCOLOR_ARGB(255,   0, 128, 128) // ����
#define DXCOLOR_MAROON        D3DCOLOR_ARGB(255, 128,   0,   0) // ����
#define DXCOLOR_PINK          D3DCOLOR_ARGB(255, 255, 192, 203) // ��ȫ��
#define DXCOLOR_LAVENDER      D3DCOLOR_ARGB(255, 230, 230, 250) // �󺥴���
#define DXCOLOR_GOLD          D3DCOLOR_ARGB(255, 255, 215,   0) // �ݻ�
#define DXCOLOR_SILVER        D3DCOLOR_ARGB(255, 192, 192, 192) // ����
#define DXCOLOR_BROWN         D3DCOLOR_ARGB(255, 165,  42,  42) // ����
#define DXCOLOR_GRAY          D3DCOLOR_ARGB(255, 128, 128, 128) // ȸ��
#define DXCOLOR_LIGHT_GRAY    D3DCOLOR_ARGB(255, 194, 194, 194) // ȸ��
#define DXCOLOR_INDIGO        D3DCOLOR_ARGB(255,  75,   0, 130) // ����
#define DXCOLOR_TURQUOISE     D3DCOLOR_ARGB(255,  64, 224, 208) // ��Ű����
#define DXCOLOR_LIME          D3DCOLOR_ARGB(255,   0, 255,   0) // ���ӻ�

}


			

#endif // Engine_Macro_h__
