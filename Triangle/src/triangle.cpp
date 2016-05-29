#include <chrono>
#include <windows.h>
#include <iostream>
#include "Triangle.h"

HWND window;
Triangle * t;

/*
* Creates and initializes a Win32 Context.
*/
void createWindowsContext(HINSTANCE hInstance, WNDPROC WndProc, std::string name, int width, int height)
{
	WNDCLASSEX wndClass;
	wndClass.cbSize        = sizeof(WNDCLASSEX);
	wndClass.style         = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc   = WndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = hInstance;
	wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = name.c_str();
	wndClass.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		std::cout << "Could not register window class!\n";
		fflush(stdout);
		exit(1);
	}

	int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle   = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	windowRect.left   = (long)screenWidth / 2 - width / 2;
	windowRect.right  = (long)width;
	windowRect.top    = (long)screenHeight / 2 - height / 2;
	windowRect.bottom = (long)height;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	window = CreateWindowEx(0,
		name.c_str(), name.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		windowRect.left, windowRect.top, windowRect.right, windowRect.bottom,
		NULL, NULL, hInstance, NULL);

	if (!window)
	{
		printf("Could not create window!\n");
		fflush(stdout);
		exit(1);
	}

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		exit(0);
		break;
	case WM_KEYDOWN:
		if (wParam == 'w')
			t->translation += t->rotation;
		else if (wParam == 's')
			t->translation -= t->rotation;
		break;
	case WM_LBUTTONDOWN:
		t->mousePos.x = (float)LOWORD(lParam);
		t->mousePos.y = (float)HIWORD(lParam);
		break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			t->rotation.x += (t->mousePos.y - (float)posy) * 1.25f * t->rotationSpeed;
			t->rotation.y -= (t->mousePos.x - (float)posx) * 1.25f * t->rotationSpeed;
			t->mousePos = glm::vec2((float)posx, (float)posy);
		}
		break;
	}

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	createWindowsContext(hInstance, WndProc, "triangle", 1280, 720);

	AllocConsole();
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD cCharsWritten;
	freopen("CONOUT$", "w", stdout);

	t = new Triangle(hInstance, window);
	t->begin();

	MSG msg;
	while (TRUE)
	{
		auto tStart = std::chrono::high_resolution_clock::now();
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);

		DispatchMessage(&msg);

		t->draw();

		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		t->frameTimer = (float) tDiff / 1000.0f;
		// Convert to clamped timer value
		if (!t->paused)
		{
			t->timer += t->timerSpeed * t->frameTimer;
			if (t->timer > 1.0)
				t->timer -= 1.0f;
		}
	}

	t->end();
	return 0;
}
