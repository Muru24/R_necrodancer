#pragma once
#include <windows.h>
#include <gdiplus.h>

class Map;
class Camera;
class UnitBase;
struct MapTile;
struct AttackEffect;

class Render
{
private:
	Render();
	~Render();

	Gdiplus::Bitmap* m_pBackgrounds[3];
	bool m_bCacheDirty;
	int m_aniPhase = 0;

	void DrawUnitInternal(Gdiplus::Graphics& graphics, Gdiplus::Image* pImg, class UnitBase& unit, Camera& camera, int srcX, int srcY, float offsetDrawY = 0.0f);

public:
	static Render& getInstance()
	{
		static Render instance;
		return instance;
	}

	void Initialize(ULONG_PTR& gdiplusToken);
	void Finalize(ULONG_PTR gdiplusToken);
	void Draw(HWND hWnd, Map& map, Camera& camera, class UnitBase& player);
	void DrawBackground(Gdiplus::Graphics& graphics, const RECT& rect, Map& map, Camera& camera);
	void DrawPlayer(Gdiplus::Graphics& graphics, class UnitBase& unit, Camera& camera);
	void DrawUnit(Gdiplus::Graphics& graphics, Camera& camera);
	void DrawUpdate(HWND hWnd, class UnitBase& unit);
	void DrawUi(Gdiplus::Graphics& graphics, UnitBase& player);
	void DrawTitle(HWND hWnd, class Title& title);
	void DrawTitleBackground(Gdiplus::Graphics& graphics);
	void DrawTitleMenuOption(Gdiplus::Graphics& graphics, int nOptionIdx, bool bSelected, bool bDraw, float x, float y);
	void DrawRhythm(Gdiplus::Graphics& graphics);
	void DrawWorldItems(Gdiplus::Graphics& graphics, Map& map, Camera& camera);
	void DrawAttackEffect(Gdiplus::Graphics& graphics, AttackEffect& effect, Camera& camera);
	void DrawBombs(Gdiplus::Graphics& graphics, Camera& camera);
	void DrawExplosions(Gdiplus::Graphics& graphics, Camera& camera);
	void DrawString(Gdiplus::Graphics& graphics, wchar_t buf[], int size, int x, int y);
	void UpdateTileCache(int x, int y, Map* pMap);
	void SetCacheDirty(bool dirty) { m_bCacheDirty = dirty; }
	void ToggleAniPhase() { m_aniPhase = (m_aniPhase + 1) % 2; }
	void InvalidateBackgroundCache() { m_bCacheDirty = true; }

	// Bridge getters for compatibility
	Gdiplus::Font* GetDefaultFont();
	Gdiplus::Font* GetBigFont();
	Gdiplus::Font* GetSelectedFont();
	Gdiplus::SolidBrush* GetWhiteBrush();
	Gdiplus::SolidBrush* GetBlackBrush();

private:
	void DrawBackgroundInternal(int index, Map& map);
};
