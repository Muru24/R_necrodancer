#include <windows.h>
#include "MainGame.h"
#include "Define.h"

#pragma comment(lib, "gdiplus.lib") 
#pragma comment(linker, "/subsystem:console") 

// 윈도우 메시지를 가공하여 각 처리기로 전달하는 콜백 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_ERASEBKGND: 
		// 더블 버퍼링을 사용하므로 배경 지우기 메시지를 무시하여 깜빡임을 방지합니다.
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

// 애플리케이션 시작점: 기본 윈도우를 생성하고 메인 루프를 실행합니다.
int main() {
	MainGame::getInstance().Initialize(); 

	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.lpszClassName = L"CadenceGame";
	RegisterClassExW(&wcex);

	// 해상도에 맞춰 윈도우 크기 조정
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowW(L"CadenceGame", L"GDI+ Sprite Sample (Refactored)", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);

	ShowWindow(hWnd, SW_SHOW);

	// 메시지 루프 및 게임 로직 업데이트 루프
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			MainGame::getInstance().Update(hWnd); 
			Sleep(10); // 프레임 제어 및 CPU 점유율 조절
		}
	}

	MainGame::getInstance().Finalize(); 
	return (int)msg.wParam;
}
