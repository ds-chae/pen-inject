// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject1.h"
#include <windows.h>
#include <iostream>
#include <thread> // For sleep
#include <vector>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HSYNTHETICPOINTERDEVICE hPointer = NULL;
HMODULE hUser32 = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Clean up
    DestroySyntheticPointerDevice(hPointer);
    FreeLibrary(hUser32);

    std::cout << "Pen input with pressure simulation injected successfully.\n";

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void simulatePenInput(HSYNTHETICPOINTERDEVICE hPointer, int startX, int startY, int endX, int endY, int steps) {
    POINTER_TYPE_INFO penInput;
    memset(&penInput, 0, sizeof(penInput));
    penInput.type = PT_PEN;
    penInput.penInfo.pointerInfo.pointerType = PT_PEN;
    penInput.penInfo.pointerInfo.pointerId = 1;

    int deltaX = (endX - startX) / steps;
    int deltaY = (endY - startY) / steps;
    int pressureMin = 500;
    int pressureMax = 2048;
    int pressureStep = (pressureMax - pressureMin) / steps;

    // Simulate Pen Down (Initial touch)
    penInput.penInfo.pointerInfo.ptPixelLocation.x = startX;
    penInput.penInfo.pointerInfo.ptPixelLocation.y = startY;
    penInput.penInfo.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    penInput.penInfo.pressure = pressureMin;

    if (!InjectSyntheticPointerInput(hPointer, &penInput, 1)) {
        std::cerr << "Failed to inject pen down input\n";
    }

    // Simulate Drag Motion with Pressure Variation
    for (int i = 1; i <= steps; i++) {
        penInput.penInfo.pointerInfo.ptPixelLocation.x = startX + deltaX * i;
        penInput.penInfo.pointerInfo.ptPixelLocation.y = startY + deltaY * i;
        penInput.penInfo.pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
        penInput.penInfo.pressure = pressureMin + pressureStep * i;

        if (!InjectSyntheticPointerInput(hPointer, &penInput, 1)) {
            std::cerr << "Failed to inject pen movement input\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Smooth transition
    }

    // Simulate Pen Up (Release)
    penInput.penInfo.pointerInfo.pointerFlags = POINTER_FLAG_UP;
    if (!InjectSyntheticPointerInput(hPointer, &penInput, 1)) {
        std::cerr << "Failed to inject pen up input\n";
    }
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
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

    // Load user32.dll for dynamic function loading
    hUser32 = LoadLibrary(L"user32.dll");
    if (!hUser32) {
        std::cerr << L"Failed to load user32.dll\n";
        return 1;
    }

    // Function pointer for InjectSyntheticPointerInput
    typedef BOOL(WINAPI* InjectSyntheticPointerInputFunc)(HANDLE, POINTER_TYPE_INFO*, UINT);
    InjectSyntheticPointerInputFunc InjectSyntheticPointerInput =
        (InjectSyntheticPointerInputFunc)GetProcAddress(hUser32, "InjectSyntheticPointerInput");

    if (!InjectSyntheticPointerInput) {
        std::cerr << "Failed to get InjectSyntheticPointerInput function\n";
        FreeLibrary(hUser32);
        return 1;
    }

    // Open a pen pointer input handle
    hPointer = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_DEFAULT);
    if (!hPointer) {
        std::cerr << "Failed to create synthetic pointer device\n";
        FreeLibrary(hUser32);
        return 1;
    }


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

#define BUTTON_ID 1  // Define Button ID
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        // Create a Button
        CreateWindow(
            L"BUTTON", L"Click Me",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            50, 50, 100, 30, hWnd, (HMENU)BUTTON_ID,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == BUTTON_ID) {
            // Simulate Pen Input (Drag from 500,500 to 700,700 with pressure change)
            simulatePenInput(hPointer, 500, 500, 700, 700, 20);
            MessageBox(hWnd, L"Button Clicked!", L"Notification", MB_OK);
            break;
        }
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
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
