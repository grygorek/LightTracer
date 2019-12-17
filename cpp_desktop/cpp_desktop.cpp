// cpp_desktop.cpp : Defines the entry point for the application.
//
#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>
#pragma comment(lib, "Gdiplus.lib")
#include "framework.h"

// remove windows crap
#ifdef min
#undef min
#endif

#include "cpp_desktop.h"
#include "display_state.h"
#include "image.h"
#include "scene.h"
#include "triangle_mesh.h"
#include "triangle_test.h"

#include <thread>

#define MAX_LOADSTRING 100
#define WM_REDRAW (WM_USER + 1)

// Global Variables:
HINSTANCE hInst;                     // current instance
WCHAR szTitle[MAX_LOADSTRING];       // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name
Image g_image{1024, 768};            // rendered image
bool g_fImageReady{false};

extern std::unique_ptr<Gdiplus::Bitmap> g_bitmap;


// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void DrawImage(HDC &hnd, const Image &image)
{
  int width{0};
  int height{0};
  for (const auto &pixel : image)
  {
    if (width >= image.width)
    {
      width = 0;
      height++;
    }
    SetPixel(hnd, width++, height, Colour(pixel));
  }
}

void OnPaint(HDC &hnd, Image &image);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Place code here.

  // Initialize global strings
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_CPPDESKTOP, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  HWND hWnd{InitInstance(hInstance, nCmdShow)};
  if (!hWnd)
  {
    return FALSE;
  }

     // Initialize GDI+.
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  // triangle_test();

  //auto start = std::chrono::high_resolution_clock::now();
  //auto mesh  = LoadPolyMeshFromFile("./geometry/backdrop.geo", IdentityMtx44f);
  //auto tt = DeltaTimeMilisec(std::chrono::high_resolution_clock::now(), start);
  //if (!mesh)
  //  OutputDebugString("Failed to load mesh\n");
  //else
  //  OutputDebugString("Load Time: " + std::to_string(tt) + "ms\n");

  std::thread t{[hWnd]() {
    scene(g_image);
    g_fImageReady = true;
    PostMessage(hWnd, WM_REDRAW, 0, 0);
  }};

  HACCEL hAccelTable =
      LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPPDESKTOP));

  MSG msg;

  // Main message loop:
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  t.join();
  g_bitmap = nullptr;
  Gdiplus::GdiplusShutdown(gdiplusToken);
  return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPPDESKTOP));
  wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName  = 0; // MAKEINTRESOURCEW(IDC_CPPDESKTOP);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance; // Store instance handle in our global variable

  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, 0, g_image.width, g_image.height,
                            nullptr, nullptr, hInstance, nullptr);

  if (!hWnd)
  {
    return nullptr;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return hWnd;
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
  DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
  if (dwStyle & WS_OVERLAPPEDWINDOW)
  {
    if (!SetFullScreen(g_image.width, g_image.height))
      return;

    WndResizeFullScreen(hwnd, dwStyle);
  }
  else
  {
    if (!ExitFullScreen())
      return;

    WndRestore(hwnd, dwStyle);
  }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_REDRAW:
    InvalidateRect(hWnd, nullptr, true);
    break;
  case WM_LBUTTONUP:
    OnLButtonUp(hWnd, 0, 0, 0);
    break;
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    if (g_fImageReady)
    {
      OnPaint(hdc, g_image);
    }
    else
    {
      const std::string s{"Rendering, plese wait..."};
      auto len = GetTabbedTextExtentA(hdc, s.c_str(), s.size(), 0, 0);
      auto w   = len & 0xFFFF;
      auto h   = len >> 16;
      TextOutA(hdc, (g_image.width - w) / 2, (g_image.height - h) / 2,
               s.c_str(), s.size());
    }
    EndPaint(hWnd, &ps);
    OutputDebugStringA("Paint\n");
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
