#pragma once
#include <windows.h>
#include <gdiplus.h>

class Map;
class Camera;
class UnitBase;
struct MapTile;

class Render
{
private:
	Render();
	~Render();

	Gdiplus::Image* m_pTileImg;
	Gdiplus::Image* m_pSpriteAtlas;
	Gdiplus::Image* m_pWallImg;
	Gdiplus::Image* m_pSlimeImg;
	Gdiplus::Image* m_HUD;
	Gdiplus::Image* m_pNote;
	Gdiplus::Image* m_pShopkeeper;

	Gdiplus::Image* m_pItemWeapons;
	Gdiplus::Image* m_pItemArmor;
	Gdiplus::Image* m_pItemHeadwear;
	Gdiplus::Image* m_pItemFootwear;
	Gdiplus::Image* m_pItemShovels;
	Gdiplus::Image* m_pItemTorches;
	Gdiplus::Image* m_pItemResources;
	Gdiplus::Image* m_pItemConsumables;
	Gdiplus::Bitmap* m_pCachedBackground;
	bool m_bCacheDirty;

	Gdiplus::Font* m_pDefaultFont;
	Gdiplus::Font* m_pBigFont;
	Gdiplus::SolidBrush* m_pWhiteBrush;
	Gdiplus::SolidBrush* m_pBlackBrush;

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

	void DrawRhythm(Gdiplus::Graphics& graphics);

	void DrawWorldItems(Gdiplus::Graphics& graphics, Map& map, Camera& camera);

	void DrawString(Gdiplus::Graphics& graphics, wchar_t buf[], int size, int x, int y);

	void UpdateTileCache(int x, int y, Map* pMap);
	void InvalidateBackgroundCache() { m_bCacheDirty = true; }

};
