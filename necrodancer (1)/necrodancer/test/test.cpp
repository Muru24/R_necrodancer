#include <windows.h>
#include "MainGame.h"
#include "Define.h"

#pragma comment(lib, "gdiplus.lib") 
#pragma comment(linker, "/subsystem:console") 

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_ERASEBKGND: 
		return 1; 

	case WM_PAINT:
	{
		MainGame::getInstance().Render(hWnd); 
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

int main() {
	MainGame::getInstance().Initialize(); 

	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.lpszClassName = L"CadenceGame";
	RegisterClassExW(&wcex);

	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowW(L"CadenceGame", L"GDI+ Sprite Sample (Refactored)", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);

	ShowWindow(hWnd, SW_SHOW);

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			MainGame::getInstance().Update(hWnd); 
			Sleep(10); 
		}
	}

	MainGame::getInstance().Finalize(); 
	return (int)msg.wParam;
}
