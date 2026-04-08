#include "Title.h"
#include "Render.h"
#include "MainGame.h"

Title::Title() : m_nSelectedIdx(0), m_fAnimTimer(0.0f), m_bIsVisible(true)
{
}

Title::~Title()
{
}

void Title::Initialize()
{
	m_nSelectedIdx = 0;
	m_fAnimTimer = 0.0f;
	m_bIsVisible = true;
}

void Title::Update()
{
	static bool bUpPrev = false;
	static bool bDownPrev = false;
	static bool bEnterPrev = false;

	bool bUp = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
	bool bDown = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
	bool bEnter = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

	if (bUp && !bUpPrev) {
		m_nSelectedIdx--;
		if (m_nSelectedIdx < 0) m_nSelectedIdx = 2;
	}
	if (bDown && !bDownPrev) {
		m_nSelectedIdx++;
		if (m_nSelectedIdx > 2) m_nSelectedIdx = 0;
	}

	if (bEnter && !bEnterPrev) {
		switch (m_nSelectedIdx) {
		case 0: 
			MainGame::getInstance().SetScene(SCENE_INGAME);
			break;
		case 1: 
			MainGame::getInstance().Initialize();
			break;
		case 2: 
			PostQuitMessage(0);
			break;
		}
	}

	bUpPrev = bUp;
	bDownPrev = bDown;
	bEnterPrev = bEnter;
}

void Title::tRender(Gdiplus::Graphics& graphics)
{
	float startY = 400.0f;
	float gap = 80.0f;

	Render& render = Render::getInstance();
	
	for (int i = 0; i < 3; ++i) {
		bool bSelected = (m_nSelectedIdx == i);
		Gdiplus::Font* pFont = bSelected ? render.GetSelectedFont() : render.GetBigFont();
		
		Gdiplus::RectF layoutRect;
		graphics.MeasureString((i == 0 ? L"START" : (i == 1 ? L"RESET" : L"EXIT")), -1, pFont, Gdiplus::PointF(0, 0), &layoutRect);

		float x = (SCREEN_WIDTH - layoutRect.Width) / 2.0f;
		float y = startY + (i * gap);

		render.DrawTitleMenuOption(graphics, i, bSelected, true, x, y);
	}
}

