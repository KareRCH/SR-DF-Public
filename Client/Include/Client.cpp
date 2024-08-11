// Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"

#include <dwmapi.h>

#define MAX_LOADSTRING 100

// �ܼ�â Ű�� ������ �̰� 1�� ����
#define _TEST_CONSOLE 1

// ���� ����:
HINSTANCE g_hInst;                              // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
HWND	g_hWnd;                                 // ������ â �����Դϴ�.
_bool   g_bLockEsc;                             // ���������� ���Ḧ �������� ���Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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
    // ����׿� �ܼ�â
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

    

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
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

    // �⺻ �޽��� �����Դϴ�.
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

            // �������� �Ѿ �� ƽ �Լ� �۵�
            if (Engine::IsPermit_Call(L"Frame", fTimeDelta_Immediate))
            {
                Engine::Set_TimeDelta(L"Timer_FPS");
                _float	fTimeDelta_60 = Engine::Get_TimeDelta(L"Timer_FPS");


                // ƽ �Լ�
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
    // �ܼ� ��� ����
    FreeConsole();
#endif
#endif // _DEBUG

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS); // ������ ���콺 Ŀ�� ����
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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

    // �׵θ� ���ֱ�
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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // ���� ����
        RECT rect;
        GetClientRect(g_hWnd, &rect);
        HDC hdc = GetDC(g_hWnd);
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        ReleaseDC(g_hWnd, hdc);

        g_bLockEsc = false;

        // ������ Ÿ��Ʋ��
        COLORREF DARK_COLOR = 0x00505050;
        BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(
            hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
            &DARK_COLOR, sizeof(DARK_COLOR)));

        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.

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

