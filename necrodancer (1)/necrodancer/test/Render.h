#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gdiplus.h>

class Map;
class Camera;
class UnitBase;

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
	
	Gdiplus::Bitmap* m_pCachedBackground;
	bool m_bCacheDirty;

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

	void DrawUi(Gdiplus::Graphics& graphics, UnitBase& player, Camera& camera);

	void DrawString(Gdiplus::Graphics& graphics,WCHAR buf[], int size, int x, int y);

	void InvalidateBackgroundCache() { m_bCacheDirty = true; }

};
