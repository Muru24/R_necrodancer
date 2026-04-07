#pragma once
#include <windows.h>
#include "Struct.h"
class Player;
class Map;
class Camera;
class Timer;
class RhythmManager;
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
	RhythmManager* m_Rhytm;

	class Title* m_pTitle;
	SceneType    m_eScene;

	DWORD lastBeatTime;
	DWORD beatInterval;
	int currentBeatCount;

public:
	static MainGame& getInstance()
	{
		static MainGame instance;
		return instance;
	}

	void Initialize();
	void Finalize();
	void Release();


	void Update(HWND hWnd);
	void Render(HWND hWnd);

	void SetScene(SceneType type) { m_eScene = type; }
	SceneType GetScene() const { return m_eScene; }

	Map* GetMap() const { return m_pMap; }
	float GetDeltaTime() const { return m_fDeltaTime; }
};
