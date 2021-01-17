#include "Include.h"
#include "Framework.h"
#include <chrono>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define WINDOW_TITLE L"Tetris"

HWND MainWindowHandle = 0;

HINSTANCE g_hInstance;

Framework dxObj;

Input g_input;

int WINAPI wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR pCmdLine,
	int pShowCmd);

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

void InitWindow(void)
{
	WNDCLASSEX wndClass;

	ZeroMemory(&wndClass, sizeof(wndClass));

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = WINDOW_TITLE;
	wndClass.hInstance = g_hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);

	// Register a new type of window
	RegisterClassEx(&wndClass);

	MainWindowHandle = CreateWindow(
		WINDOW_TITLE, WINDOW_TITLE,
		false ? WS_OVERLAPPEDWINDOW | WS_VISIBLE : (WS_POPUP | WS_VISIBLE),
		CW_USEDEFAULT, CW_USEDEFAULT,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, g_hInstance, 0);

	// Display the window
	ShowWindow(MainWindowHandle, SW_SHOW);

	UpdateWindow(MainWindowHandle);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	hInstance = g_hInstance;
	bool g_bWindowed = false;	// Windowed or full-screen

	// Initialize once
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	// Hide cursor
	ShowCursor(false);

	// Init the window
	InitWindow();

	// msg structure to catch window messages
	MSG msg;

	ZeroMemory(&msg, sizeof(msg));

	// Initialize controller class
	dxObj.Init(MainWindowHandle, hInstance, &g_input, g_bWindowed);

	float dt, elapsed(0.0f);

	const float fps(60.0f);

	const float increment = (1000.0f / fps);

	std::chrono::high_resolution_clock::time_point t1, t2(std::chrono::high_resolution_clock::now());

	std::chrono::duration<float, std::milli> delta;

	// Main game loop
	while (msg.message != WM_QUIT)
	{
		t1 = std::chrono::high_resolution_clock::now();

		delta = t1 - t2;

		elapsed += dt = delta.count();

		t2 = t1;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (elapsed >= increment)
		{
			dxObj.Update(dt);
			
			elapsed -= increment;
		}

		dxObj.Render();

		Sleep(1);
	}

	dxObj.Shutdown();

	UnregisterClass(WINDOW_TITLE, hInstance);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_PAINT:
		InvalidateRect(hWnd, NULL, TRUE);

		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);

		break;
	
	case WM_KEYDOWN:
		g_input.keyboard[(int)wparam] = true;

		break;
		

	case WM_KEYUP:
		g_input.keyboard[(int)wparam] = false;

		break;
	}

	g_input.key = wparam;

	return DefWindowProc(hWnd, message, wparam, lparam);
}
