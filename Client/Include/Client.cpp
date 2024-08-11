// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"

#include <dwmapi.h>

#define MAX_LOADSTRING 100

// 콘솔창 키고 싶으면 이거 1로 설정
#define _TEST_CONSOLE 1

// 전역 변수:
HINSTANCE g_hInst;                              // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND	g_hWnd;                                 // 윈도우 창 변수입니다.
_bool   g_bLockEsc;                             // 비정상적인 종료를 막기위한 락입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
#if _TEST_CONSOLE
    // 디버그용 콘솔창
    if (::AllocConsole() == TRUE)
    {
        FILE* nfp[3];
        freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
        freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
        freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
        std::ios::sync_with_stdio();
    }
#endif
#endif // _DEBUG

    

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;
	msg.message = WM_NULL;

	CMainApp*		pMainApp = CMainApp::Create();
	
	if (nullptr == pMainApp)
		return FALSE;

    FAILED_CHECK_RETURN(Engine::Ready_Timer(L"Timer_Immediate"), FALSE);
    FAILED_CHECK_RETURN(Engine::Ready_Timer(L"Timer_FPS"), FALSE);

    FAILED_CHECK_RETURN(Engine::Ready_Frame(L"Frame", 300.f), FALSE);

    // 기본 메시지 루프입니다.
    while (true)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message && !g_bLockEsc)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
            Engine::Set_TimeDelta(L"Timer_Immediate");

            _float	fTimeDelta_Immediate = Engine::Get_TimeDelta(L"Timer_Immediate");

            // 프레임이 넘어갈 때 틱 함수 작동
            if (Engine::IsPermit_Call(L"Frame", fTimeDelta_Immediate))
            {
                Engine::Set_TimeDelta(L"Timer_FPS");
                _float	fTimeDelta_60 = Engine::Get_TimeDelta(L"Timer_FPS");


                // 틱 함수
                pMainApp->Update_MainApp(fTimeDelta_60);
                pMainApp->LateUpdate_MainApp();
                pMainApp->Render_MainApp();
            }
		}
    }

	_ulong dwRefCnt = 0;

    if (dwRefCnt = Engine::Safe_Release(pMainApp))
    {
        MSG_BOX("MainApp Destroy Failed");
        return FALSE;
    }

#ifdef _DEBUG
#if _TEST_CONSOLE
    // 콘솔 사용 해제
    FreeConsole();
#endif
#endif // _DEBUG

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS); // 윈도우 마우스 커서 변경
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    RECT	rc{ 0, 0, WINCX, WINCY };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 
	    rc.right - rc.left, 
	    rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

    /*DEVMODE devmode = {};
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
    HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW, szWindowClass, szTitle, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, devmode.dmPelsWidth, devmode.dmPelsHeight, nullptr, nullptr, hInstance, nullptr);*/

    // 테두리 없애기
    //SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hWnd = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 눈뽕 방지
        RECT rect;
        GetClientRect(g_hWnd, &rect);
        HDC hdc = GetDC(g_hWnd);
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        ReleaseDC(g_hWnd, hdc);

        g_bLockEsc = false;

        // 검은색 타이틀바
        COLORREF DARK_COLOR = 0x00505050;
        BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(
            hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
            &DARK_COLOR, sizeof(DARK_COLOR)));

        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {        
            case IDM_EXIT:
                if (!g_bLockEsc)
                    DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.

            RECT rect;
            GetClientRect(g_hWnd, &rect);
            HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);

            EndPaint(hWnd, &ps);
        }
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

