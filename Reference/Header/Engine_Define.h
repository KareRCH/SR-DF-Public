#ifndef Engine_Define_h__
#define Engine_Define_h__

#pragma warning(disable : 26495)

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <array>
#include <tuple>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>


// 사운드 라이브러리
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmod_studio.h"
#include "fmod_studio.hpp"

#include <io.h>
#include "fmod_common.h"

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)


#ifndef __IMGUI
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

#else
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#endif

using namespace std;
using namespace Engine;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern _bool g_bLockEsc;		// 비정상 종료를 막기위한 플래그

#endif // Engine_Define_h__
