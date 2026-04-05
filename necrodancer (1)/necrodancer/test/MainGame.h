#pragma once
#include <windows.h>

class Player;
class Map;
class Camera;
class Timer;

class MainGame
{
private:
	MainGame();
	~MainGame();

	Player*     m_pPlayer;
	Map*        m_pMap;
	Camera*     m_pCamera;
	Timer*      m_pTimer;
	float       m_fDeltaTime;
	ULONG_PTR   m_gdiplusToken;

public:
	static MainGame& getInstance()
	{
		static MainGame instance;
		return instance;
	}

	void Initialize();
	void Finalize();


	void Update(HWND hWnd);
	void Render(HWND hWnd);

	Map* GetMap() const { return m_pMap; }
	float GetDeltaTime() const { return m_fDeltaTime; }
};
