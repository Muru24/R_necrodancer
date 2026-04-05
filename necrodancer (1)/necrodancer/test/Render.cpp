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
#include "Light.h"

using namespace Gdiplus;

Render::Render() : m_pTileImg(nullptr), m_pSpriteAtlas(nullptr), m_pWallImg(nullptr), m_pSlimeImg(nullptr), m_HUD(nullptr),
m_pCachedBackground(nullptr), m_bCacheDirty(true),
m_pDefaultFont(nullptr), m_pBigFont(nullptr), m_pWhiteBrush(nullptr), m_pBlackBrush(nullptr)
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
	m_HUD = new Gdiplus::Image(SPRITEPATH_HUD);

	m_pDefaultFont = new Gdiplus::Font(L"맑은 고딕", 12, FontStyleBold);
	m_pBigFont = new Gdiplus::Font(L"맑은 고딕", 24, FontStyleBold);
	m_pWhiteBrush = new Gdiplus::SolidBrush(Color(255, 255, 255, 255));
	m_pBlackBrush = new Gdiplus::SolidBrush(Color(255, 0, 0, 0));
}

void Render::Finalize(ULONG_PTR gdiplusToken)
{
	if (m_pSpriteAtlas)    delete m_pSpriteAtlas;
	if (m_pTileImg)        delete m_pTileImg;
	if (m_pWallImg)        delete m_pWallImg;
	if (m_pSlimeImg)       delete m_pSlimeImg;
	if (m_pCachedBackground) delete m_pCachedBackground;
	if (m_HUD) delete m_HUD;

	if (m_pDefaultFont) delete m_pDefaultFont;
	if (m_pBigFont) delete m_pBigFont;
	if (m_pWhiteBrush) delete m_pWhiteBrush;
	if (m_pBlackBrush) delete m_pBlackBrush;

	GdiplusShutdown(gdiplusToken);
}


void Render::DrawString(Gdiplus::Graphics& graphics, WCHAR buf[], int size, int x, int y)
{
	if (!m_pBigFont || !m_pWhiteBrush || !m_pBlackBrush) return;

	Gdiplus::PointF point((float)x, (float)y);

	for (int ox = -1; ox <= 1; ++ox) {
		for (int oy = -1; oy <= 1; ++oy) {
			if (ox == 0 && oy == 0) continue;
			graphics.DrawString(buf, -1, m_pBigFont, PointF(point.X + ox, point.Y + oy), m_pBlackBrush);
		}
	}
	graphics.DrawString(buf, -1, m_pBigFont, point, m_pWhiteBrush);
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
							(REAL)TILE_DEFAULT_X, (REAL)TILE_DEFAULT_Y, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE,
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_DEFAULT) {
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

	// 전장의 안개 (Fog of War) 및 시야 효과 적용
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			VisibilityType vis = Light::getInstance().GetVisibility(x, y);

			if (vis == VIS_VISIBLE) continue;

			float drawX = (float)x * gridSize - camX;
			float drawY = (float)y * gridSize - camY;

			if (vis == VIS_HIDDEN) {
				// 미탐사 구역: 완전 검정
				SolidBrush blackBrush(Color(255, 0, 0, 0));
				graphics.FillRectangle(&blackBrush, RectF(drawX, drawY, gridSize, gridSize));
			}
			else if (vis == VIS_EXPLORED) {
				// 탐사된 구역: 반투명 검정 레이어 (어둡게 처리)
				SolidBrush shadowBrush(Color(180, 0, 0, 0));
				graphics.FillRectangle(&shadowBrush, RectF(drawX, drawY, gridSize, gridSize));
			}
		}
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
			// 적 유닛은 시야가 VIS_VISIBLE일 때만 출력
			int gridX = (int)(unit->GetX() / (FRAME_SIZE * DRAW_SCALE));
			int gridY = (int)(unit->GetY() / (FRAME_SIZE * DRAW_SCALE));
			if (Light::getInstance().GetVisibility(gridX, gridY) != VIS_VISIBLE) continue;

			int currentFrame = unit->GetCurrentFrame();
			int srcX = currentFrame * FRAME_SIZE + 5;
			int srcY = 0;

			DrawUnitInternal(graphics, m_pSlimeImg, *unit, camera, srcX, srcY);
		}

		// 실시간 디버그 정보 (HP 등) 출력
		float unitX = (float)unit->GetX() - camera.GetX();
		float unitY = (float)unit->GetY() - camera.GetY();

		// 적 유닛의 경우 시야(VIS_VISIBLE) 내에 있을 때만 디버그 정보 출력
		if (unit->GetTag() == ENEMY) {
			int gridX = (int)(unit->GetX() / (FRAME_SIZE * DRAW_SCALE));
			int gridY = (int)(unit->GetY() / (FRAME_SIZE * DRAW_SCALE));
			if (Light::getInstance().GetVisibility(gridX, gridY) != VIS_VISIBLE) continue;
		}

		WCHAR szBuf[128];
		swprintf_s(szBuf, L"HP: %d/%d\nATK: %d", unit->GetHp(), unit->GetMaxHp(), unit->GetAttack());

		Gdiplus::PointF point(unitX, unitY - 30.0f);

		if (m_pDefaultFont && m_pWhiteBrush && m_pBlackBrush) {
			// 외곽선 효과 (검은색)
			for (int ox = -1; ox <= 1; ++ox) {
				for (int oy = -1; oy <= 1; ++oy) {
					if (ox == 0 && oy == 0) continue;
					graphics.DrawString(szBuf, -1, m_pDefaultFont, PointF(point.X + ox, point.Y + oy), m_pBlackBrush);
				}
			}
			graphics.DrawString(szBuf, -1, m_pDefaultFont, point, m_pWhiteBrush);
		}
	}
}

void Render::Draw(HWND hWnd, Map& map, Camera& camera, UnitBase& player)
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
		DrawUi(graphics, player, camera);
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

void Render::DrawUi(Gdiplus::Graphics& graphics, UnitBase& player, Camera& camera)
{
	if (!m_HUD || m_HUD->GetLastStatus() != Ok) return;

	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	float drawSize = (float)(FRAME_SIZE * DRAW_SCALE);
	int hp = player.GetHp();
	int maxHp = player.GetMaxHp();

	float startX = UI_HP_POS_X;
	float startY = UI_HP_POS_Y;
	float offset = drawSize * 1.05f;

	for (int i = 0; i < maxHp; ++i)
	{
		float srcX, srcY;
		if (i < hp) {
			srcX = (float)UI_HP_SCR_X;
			srcY = (float)UI_HP_SCR_Y;
		}
		else {
			srcX = (float)UI_NULL_HP_SCR_X;
			srcY = (float)UI_NULL_HP_SCR_Y;
		}

		int xMultiplier = i % 5;
		int rowIndex = (i / 5) * 8;

		float currentY = startY + (rowIndex * 10.0f);

		graphics.DrawImage(m_HUD,
			RectF(startX - (xMultiplier * offset), currentY, drawSize, drawSize),
			(REAL)srcX, (REAL)srcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE,
			UnitPixel);
	}

	float uiDrawHeight = drawSize * UI_DRAW_SCALE;
	float uiDrawWidth = uiDrawHeight * (UI_INVEN_FRAME_X / (float)UI_INVEN_FRAME_Y);
	for (int i = 0; i < 6; i++)
	{
		float destX = UI_INVEN_POS_X + i * (uiDrawWidth + UI_GAP);
		float srcX = (float)(UI_INVEN_PNG_X + i * (UI_INVEN_FRAME_X + UI_GAP));
		graphics.DrawImage(m_HUD,
			RectF(destX, UI_INVEN_POS_Y, uiDrawWidth, uiDrawHeight),
			(REAL)srcX, (REAL)UI_INVEN_PNG_Y, (REAL)UI_INVEN_FRAME_X, (REAL)UI_INVEN_FRAME_Y,
			UnitPixel);
	}

	for (int i = 0; i < 2; i++)
	{
		float destY = UI_ITEM_POS_Y + i * (uiDrawHeight + UI_GAP);
		float srcY = (float)(UI_ITEM_PNG_Y + i * (UI_ITEM_FRAME_PNG_Y + UI_ITEM_GAP_Y));
		graphics.DrawImage(m_HUD,
			RectF(UI_ITEM_POS_X, destY, uiDrawWidth, uiDrawHeight),
			(REAL)UI_ITEM_PNG_X, (REAL)srcY, (REAL)UI_ITEM_FRAME_PNG_X, (REAL)UI_ITEM_FRAME_PNG_Y,
			UnitPixel);
	}

	graphics.DrawImage(m_HUD,
		RectF(UI_MONEY_POS_X, UI_MONEY_POS_Y, drawSize, drawSize),
		(REAL)UI_MONEY_SCR_X, (REAL)UI_MONEY_SCR_Y, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE,
		UnitPixel);

	graphics.DrawImage(m_HUD,
		RectF(UI_JEWEL_POS_X, UI_JEWEL_POS_Y, drawSize, drawSize),
		(REAL)UI_JEWEL_SCR_X, (REAL)UI_JEWEL_SCR_Y, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE,
		UnitPixel);

	WCHAR szBuf[128];
	swprintf_s(szBuf, L" x%d", 0);
	DrawString(graphics, szBuf,24, UI_MONEY_FONT_POS_X, UI_MONEY_FONT_POS_Y);
	DrawString(graphics, szBuf,24, UI_JEWEL_FONT_POS_X, UI_JEWEL_FONT_POS_Y);
	
}
