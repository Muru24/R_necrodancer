#include "Define.h"
#include "Render.h"
#include "UnitBase.h"
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include "ObjectContainer.h"
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace Gdiplus;

Render::Render() : m_pTileImg(nullptr), m_pSpriteAtlas(nullptr), m_pWallImg(nullptr), m_pSlimeImg(nullptr),
		m_pCachedBackground(nullptr), m_bCacheDirty(true)
{
}

Render::~Render()
{
}

void Render::Initialize(ULONG_PTR& gdiplusToken)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_pSpriteAtlas = new Gdiplus::Image(SPRITEPATH_CHARACTERS);
	m_pTileImg = new Gdiplus::Image(SPRITEPATH_FLOORS);
	m_pWallImg = new Gdiplus::Image(SPRITEPATH_WALLS);
	m_pSlimeImg = new Gdiplus::Image(SPRITEPATH_SLIMES);
}

void Render::Finalize(ULONG_PTR gdiplusToken)
{
	if (m_pSpriteAtlas)    delete m_pSpriteAtlas;
	if (m_pTileImg)        delete m_pTileImg;
	if (m_pWallImg)        delete m_pWallImg;
	if (m_pSlimeImg)       delete m_pSlimeImg;
	if (m_pCachedBackground) delete m_pCachedBackground;

	GdiplusShutdown(gdiplusToken);
}

void Render::DrawBackground(Gdiplus::Graphics& graphics, const RECT& rect, Map& map, Camera& camera)
{
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	float camX = camera.GetX();
	float camY = camera.GetY();

	if (m_bCacheDirty || !m_pCachedBackground) {
		if (m_pCachedBackground) {
			delete m_pCachedBackground;
			m_pCachedBackground = nullptr;
		}

		m_pCachedBackground = new Gdiplus::Bitmap((int)(MAP_WIDTH * gridSize), (int)(MAP_HEIGHT * gridSize), PixelFormat32bppARGB);
		Gdiplus::Graphics cacheG(m_pCachedBackground);
		cacheG.SetInterpolationMode(InterpolationModeNearestNeighbor);
		cacheG.Clear(Color(255, 0, 0, 0));

		if (m_pTileImg && m_pWallImg && m_pTileImg->GetLastStatus() == Ok && m_pWallImg->GetLastStatus() == Ok) {
			for (int y = 0; y < MAP_HEIGHT; y++) {
				for (int x = 0; x < MAP_WIDTH; x++) {
					MapTile tile = map.GetTile(x, y);

					if (tile.type == TILE_FLOOR) {
						cacheG.DrawImage(m_pTileImg,
							RectF((float)x * gridSize, (float)y * gridSize, gridSize, gridSize),
							(REAL)Tile_DEFAULT_X, (REAL)Tile_DEFAULT_Y, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE,
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_DEFULT) {
						REAL srcX = (REAL)WALL_DEFAULT_X + (tile.variant * FRAME_SIZE);
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							srcX, (REAL)WALL_DEFAULT_Y, (REAL)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_HARD) {
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							(REAL)WALL_HARD_X, (REAL)WALL_HARD_Y, (REAL)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_BADROCK) {
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							(REAL)WALL_BADROCK_X, (REAL)WALL_BADROCK_Y, (REAL)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_SHOP) {
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							(REAL)WALL_SHOP_X, (REAL)WALL_SHOP_Y, (REAL)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
				}
			}
		}

		Pen gridPen(Color(120, 255, 255, 255), 2.0f);
		for (float x = 0; x <= (float)(MAP_WIDTH * gridSize); x += gridSize) {
			cacheG.DrawLine(&gridPen, (REAL)x, 0.0f, (REAL)x, (REAL)(MAP_HEIGHT * gridSize));
		}
		for (float y = 0; y <= (float)(MAP_HEIGHT * gridSize); y += gridSize) {
			cacheG.DrawLine(&gridPen, 0.0f, (REAL)y, (REAL)(MAP_WIDTH * gridSize), (REAL)y);
		}

		m_bCacheDirty = false;
	}

	if (m_pCachedBackground) {
		graphics.DrawImage(m_pCachedBackground, -camX, -camY);
	}
}

void Render::DrawPlayer(Graphics& graphics, ::UnitBase& unit, Camera& camera)
{
	if (!m_pSpriteAtlas || m_pSpriteAtlas->GetLastStatus() != Ok) return;

	int currentFrame = unit.GetCurrentFrame();

	int bSrcX = currentFrame * FRAME_SIZE;
	int bSrcY = FRAME_SIZE * 2;
	int hSrcX = currentFrame * FRAME_SIZE;
	int hSrcY = 0;

	DrawUnitInternal(graphics, m_pSpriteAtlas, unit, camera, bSrcX, bSrcY, (float)DRAW_SCALE);
	DrawUnitInternal(graphics, m_pSpriteAtlas, unit, camera, hSrcX, hSrcY, 0.0f);
}

void Render::DrawUnitInternal(Gdiplus::Graphics& graphics, Gdiplus::Image* pImg, class UnitBase& unit, Camera& camera, int srcX, int srcY, float offsetDrawY)
{
	if (!pImg || pImg->GetLastStatus() != Ok) return;

	int camX = camera.GetX();
	int camY = camera.GetY();

	float unitX = (float)unit.GetX() - camX;
	float unitY = (float)unit.GetY() - camY;

	float jumpOffsetY = 0.0f;
	if (unit.GetIsMoving()) {
		float progress = unit.GetMoveProgress();
		float jumpHeight = unit.GetJumpHeight();
		jumpOffsetY = sinf(progress * 3.141592f) * jumpHeight;
	}

	bool isLookLeft = unit.GetIsLookLeft();
	float drawWidth = (float)(FRAME_SIZE * DRAW_SCALE);
	float drawHeight = (float)(FRAME_SIZE * DRAW_SCALE);

	if (isLookLeft) {
		graphics.DrawImage(pImg,
			RectF(unitX + drawWidth, (unitY - jumpOffsetY) + offsetDrawY, -drawWidth, drawHeight),
			(REAL)srcX, (REAL)srcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE, UnitPixel);
	}
	else {
		graphics.DrawImage(pImg,
			RectF(unitX, (unitY - jumpOffsetY) + offsetDrawY, drawWidth, drawHeight),
			(REAL)srcX, (REAL)srcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE, UnitPixel);
	}
}

void Render::DrawUnit(Gdiplus::Graphics& graphics, Camera& camera)
{
	const auto& unitContainer = ObjectContainer::getInstance().GetUnitContainer();

	for (auto* unit : unitContainer)
	{
		if (!unit || !unit->GetIsAlive()) continue;

		if (unit->GetTag() == PLAYER)
		{
			DrawPlayer(graphics, *unit, camera);
		}
		else if (unit->GetTag() == ENEMY)
		{
			int currentFrame = unit->GetCurrentFrame();
			int srcX = currentFrame * FRAME_SIZE + 5;
			int srcY = 0;

			DrawUnitInternal(graphics, m_pSlimeImg, *unit, camera, srcX, srcY);
		}

		// 실시간 디버그 정보 (HP 등) 출력
		float unitX = (float)unit->GetX() - camera.GetX();
		float unitY = (float)unit->GetY() - camera.GetY();

		WCHAR szBuf[128];
		swprintf_s(szBuf, L"HP: %d/%d\nATK: %d", unit->GetHp(), (unit->GetTag() == PLAYER ? 10 : 5), unit->GetAttack());

		Gdiplus::Font font(L"맑은 고딕", 12, FontStyleBold);
		Gdiplus::SolidBrush brush(Color(255, 255, 255, 255));
		Gdiplus::PointF point(unitX, unitY - 30.0f);

		// 외곽선 효과 (검은색)
		Gdiplus::SolidBrush shadowBrush(Color(255, 0, 0, 0));
		for (int ox = -1; ox <= 1; ++ox) {
			for (int oy = -1; oy <= 1; ++oy) {
				if (ox == 0 && oy == 0) continue;
				graphics.DrawString(szBuf, -1, &font, PointF(point.X + ox, point.Y + oy), &shadowBrush);
			}
		}
		graphics.DrawString(szBuf, -1, &font, point, &brush);
	}
}

void Render::Draw(HWND hWnd, Map& map, Camera& camera)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	RECT rect;
	GetClientRect(hWnd, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBitmap = CreateCompatibleBitmap(hdc, w, h);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

	HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30));
	FillRect(memDC, &rect, hBrush);
	DeleteObject(hBrush);

	{
		Graphics graphics(memDC);
		DrawBackground(graphics, rect, map, camera);
		DrawUnit(graphics, camera);
	}

	BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteObject(memBitmap);
	DeleteDC(memDC);
	EndPaint(hWnd, &ps);
}

void Render::DrawUpdate(HWND hWnd, UnitBase& player)
{
	InvalidateRect(hWnd, NULL, FALSE);
}
