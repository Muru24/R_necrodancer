#pragma once
#include <windows.h>

class Player;
class Map;
class Camera;
class Timer;

// 게임의 전체적인 흐름(초기화, 업데이트, 렌더링, 해제)을 관리하는 메인 클래스 (싱글톤)
class MainGame
{
private:
	MainGame();
	~MainGame();

	Player*     m_pPlayer;      // 플레이어 객체
	Map*        m_pMap;         // 맵 객체
	Camera*     m_pCamera;      // 카메라 객체
	Timer*      m_pTimer;       // 타이머 객체
	ULONG_PTR   m_gdiplusToken; // GDI+ 초기화 토큰

public:
	static MainGame& getInstance()
	{
		static MainGame instance;
		return instance;
	}

	// 게임에 필요한 모든 객체와 자원을 초기화합니다.
	void Initialize();

	// 사용된 모든 자원을 해제하고 종료 처리를 수행합니다.
	void Finalize();

	// 매 프레임 게임 로직(이동, 카메라 추적 등)을 갱신합니다.
	// - hWnd: 입력 및 출력 대상 윈도우 핸들
	void Update(HWND hWnd);

	// 현재 게임 상태를 화면에 그립니다.
	// - hWnd: 출력 대상 윈도우 핸들
	void Render(HWND hWnd);
};
