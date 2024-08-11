// Tool.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//


#include "framework.h"

#include "Tool.h"
#include "MainApp.h"

#include <dwmapi.h>

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#define MAX_LOADSTRING 100

// 콘솔창 키고 싶으면 이거 1로 설정
#define _TEST_CONSOLE 1

// 전역 변수: 어플리케이션에서 윈도우를 제어할 때는 창을 제어한다.
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;
UINT g_ResizeWidth = 0, g_ResizeHeight = 0;     // 창 변화시 사이즈를 변경해 주기 위한 변수
_bool   g_bLockEsc;                             // 비정상적인 종료를 막기위한 락입니다.
CMainApp* g_pMainApp = nullptr;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

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

    // 전역 문자열을 초기화합니다.
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TOOL, szWindowClass, MAX_LOADSTRING);
    lstrcpy(szTitle, L"FallenAces Tool");
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOOL));

    MSG msg;
    msg.message = WM_NULL;

    CMainApp* pMainApp = g_pMainApp = CMainApp::Create();

    if (nullptr == pMainApp)
        return FALSE;

    FAILED_CHECK_RETURN(Engine::Ready_Timer(L"Timer_Immediate"), FALSE);
    FAILED_CHECK_RETURN(Engine::Ready_Timer(L"Timer_FPS60"), FALSE);

    FAILED_CHECK_RETURN(Engine::Ready_Frame(L"Frame60", 165.f), FALSE);

    // 기본 메시지 루프입니다.
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == msg.message)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // Handle window resize (we don't resize directly in the WM_SIZE handler)
            if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
            {
                pMainApp->ResetDevice(g_ResizeWidth, g_ResizeHeight);
                g_ResizeWidth = g_ResizeHeight = 0;
            }

            Engine::Set_TimeDelta(L"Timer_Immediate");

            _float	fTimeDelta_Immediate = Engine::Get_TimeDelta(L"Timer_Immediate");

            // 프레임이 넘어갈 때 틱 함수 작동
            if (Engine::IsPermit_Call(L"Frame60", fTimeDelta_Immediate))
            {
                Engine::Set_TimeDelta(L"Timer_FPS60");
                _float	fTimeDelta_60 = Engine::Get_TimeDelta(L"Timer_FPS60");

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

    //_CrtDumpMemoryLeaks(); 메모리 누수 잡는 코드 지금은 없어도 잘 됨
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_CLASSDC;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_METAL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(CreateSolidBrush(RGB(28, 28, 28)));
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = NULL;

    //DeleteObject(wcex.hbrBackground);

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT	rc{ 0, 0, WINCX, WINCY };

   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   // WS_POPUP : 타이틀바 없애기
   HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

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
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_CREATE:
    {
        COLORREF DARK_COLOR = 0x00505050;
        BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(
            hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
            &DARK_COLOR, sizeof(DARK_COLOR)));
        break;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
            return 0;

        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;

        //HDC hdc = GetDC(hWnd); // 윈도우의 DC 얻기

        //// 빨간색으로 채우기 (255, 0, 0은 각각 R, G, B를 나타냄)
        //ImVec4 clear_color = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        //HBRUSH redBrush = CreateSolidBrush(RGB((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f)));
        //FillRect(hdc, &clientRect, redBrush);
        //DeleteObject(redBrush);

        ////::SetWindowPos(hWnd, nullptr, clientRect.left, clientRect.top, clientWidth, clientHeight, SWP_NOZORDER | SWP_NOACTIVATE);

        //ReleaseDC(hWnd, hdc); // DC 반환

        // 디바이스 초기화가 되었을 때
        if (Engine::Get_DeviceAvailable() == S_OK)
        {
            ImVec4 clear_color = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
            D3DXCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
            
            LPDIRECT3DSURFACE9 pSurface = NULL;

            Get_GraphicDev()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);

            Get_GraphicDev()->Clear(0, NULL, D3DCLEAR_TARGET, clear_col_dx, 1.f, 0);

            Get_GraphicDev()->StretchRect(pSurface, NULL, NULL, NULL, D3DTEXF_NONE);

            pSurface->Release();
        }
        break;
    }
        
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
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
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
