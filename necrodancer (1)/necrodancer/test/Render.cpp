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
#include "RhythmManager.h"
#include "R_Note.h"
#include "Title.h"

using namespace Gdiplus;

Render::Render() : m_pTileImg(nullptr), m_pSpriteAtlas(nullptr), m_pWallImg(nullptr), m_pSlimeImg(nullptr), m_HUD(nullptr),
m_pShopkeeper(nullptr), m_pNote(nullptr),
m_pItemWeapons(nullptr), m_pItemArmor(nullptr), m_pItemHeadwear(nullptr), m_pItemFootwear(nullptr),
m_pItemShovels(nullptr), m_pItemTorches(nullptr), m_pItemResources(nullptr), m_pItemConsumables(nullptr),
m_pTitleImg(nullptr), m_pCachedBackground(nullptr), m_bCacheDirty(true),
m_pDefaultFont(nullptr), m_pBigFont(nullptr), m_pSelectedFont(nullptr), m_pWhiteBrush(nullptr), m_pBlackBrush(nullptr)
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
	m_pShopkeeper = new Gdiplus::Image(SPRITEPATH_SHOPKEPPER);

	m_pDefaultFont = new Gdiplus::Font(L"맑은 고딕", 12, FontStyleBold);
	m_pBigFont = new Gdiplus::Font(L"맑은 고딕", 24, FontStyleBold);
	m_pSelectedFont = new Gdiplus::Font(L"맑은 고딕", 32, FontStyleBold);
	m_pWhiteBrush = new Gdiplus::SolidBrush(Color(255, 255, 255, 255));
	m_pBlackBrush = new Gdiplus::SolidBrush(Color(255, 0, 0, 0));

	m_pItemWeapons = new Gdiplus::Image(SPRITEPATH_WEAPONS);
	m_pItemArmor = new Gdiplus::Image(SPRITEPATH_ARMOR);
	m_pItemHeadwear = new Gdiplus::Image(SPRITEPATH_HEADWEAR);
	m_pItemFootwear = new Gdiplus::Image(SPRITEPATH_FOOTWEAR);
	m_pItemShovels = new Gdiplus::Image(SPRITEPATH_SHOVELS);
	m_pItemTorches = new Gdiplus::Image(SPRITEPATH_TORCHES);
	m_pItemResources = new Gdiplus::Image(SPRITEPATH_RESOURCES);
	m_pItemConsumables = new Gdiplus::Image(SPRITEPATH_CONSUMABLES);
	m_pTitleImg = new Gdiplus::Image(SPRITEPATH_SCENE_TITLE);
}

void Render::Finalize(ULONG_PTR gdiplusToken)
{
	if (m_pSpriteAtlas)    delete m_pSpriteAtlas;
	if (m_pTileImg)        delete m_pTileImg;
	if (m_pWallImg)        delete m_pWallImg;
	if (m_pSlimeImg)       delete m_pSlimeImg;
	if (m_pCachedBackground) delete m_pCachedBackground;
	if (m_HUD) delete m_HUD;
	if (m_pShopkeeper) delete m_pShopkeeper;
	if (m_pNote) delete m_pNote;

	if (m_pDefaultFont) delete m_pDefaultFont;
	if (m_pBigFont) delete m_pBigFont;
	if (m_pSelectedFont) delete m_pSelectedFont;
	if (m_pWhiteBrush) delete m_pWhiteBrush;
	if (m_pBlackBrush) delete m_pBlackBrush;

	if (m_pItemWeapons) delete m_pItemWeapons;
	if (m_pItemArmor) delete m_pItemArmor;
	if (m_pItemHeadwear) delete m_pItemHeadwear;
	if (m_pItemFootwear) delete m_pItemFootwear;
	if (m_pItemShovels) delete m_pItemShovels;
	if (m_pItemTorches) delete m_pItemTorches;
	if (m_pItemResources) delete m_pItemResources;
	if (m_pItemConsumables) delete m_pItemConsumables;
	if (m_pTitleImg)        delete m_pTitleImg;

	GdiplusShutdown(gdiplusToken);
}

void Render::DrawString(Gdiplus::Graphics& graphics, wchar_t buf[], int size, int x, int y)
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
							(float)TILE_DEFAULT_X, (float)TILE_DEFAULT_Y, (float)FRAME_SIZE, (float)FRAME_SIZE,
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_DEFAULT) {
						float srcX = (float)WALL_DEFAULT_X + (tile.variant * FRAME_SIZE);
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							srcX, (float)WALL_DEFAULT_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_HARD) {
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							(float)WALL_HARD_X, (float)WALL_HARD_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_BADROCK) {
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							(float)WALL_BADROCK_X, (float)WALL_BADROCK_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
					else if (tile.type == TILE_WALL_SHOP) {
						cacheG.DrawImage(m_pWallImg,
							RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
							(float)WALL_SHOP_X, (float)WALL_SHOP_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
							UnitPixel);
					}
				}
			}
		}

		m_bCacheDirty = false;
	}

	if (m_pCachedBackground) {
		graphics.DrawImage(m_pCachedBackground, -camX, -camY);
	}

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			Visibility vis = Light::getInstance().GetVisibility(x, y);

			if (vis == VIS_VISIBLE) continue;

			float drawX = (float)x * gridSize - camX;
			float drawY = (float)y * gridSize - camY;

			if (vis == VIS_HIDDEN) {
				SolidBrush blackBrush(Color(255, 0, 0, 0));
				graphics.FillRectangle(&blackBrush, RectF(drawX, drawY, gridSize, gridSize));
			}
			else if (vis == VIS_EXPLORED) {
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
			(float)srcX, (float)srcY, (float)FRAME_SIZE, (float)FRAME_SIZE, UnitPixel);
	}
	else {
		graphics.DrawImage(pImg,
			RectF(unitX, (unitY - jumpOffsetY) + offsetDrawY, drawWidth, drawHeight),
			(float)srcX, (float)srcY, (float)FRAME_SIZE, (float)FRAME_SIZE, UnitPixel);
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
			int gridX = (int)(unit->GetX() / (FRAME_SIZE * DRAW_SCALE));
			int gridY = (int)(unit->GetY() / (FRAME_SIZE * DRAW_SCALE));
			if (Light::getInstance().GetVisibility(gridX, gridY) != VIS_VISIBLE) continue;

			int currentFrame = unit->GetCurrentFrame();
			int srcX = currentFrame * FRAME_SIZE + 5;
			int srcY = 0;

			DrawUnitInternal(graphics, m_pSlimeImg, *unit, camera, srcX, srcY);
		}
		else if (unit->GetTag() == NPC)
		{
			int gridX = (int)(unit->GetX() / (FRAME_SIZE * DRAW_SCALE));
			int gridY = (int)(unit->GetY() / (FRAME_SIZE * DRAW_SCALE));
			if (Light::getInstance().GetVisibility(gridX, gridY) != VIS_VISIBLE) continue;
			if (!m_pShopkeeper || m_pShopkeeper->GetLastStatus() != Ok) continue;

			float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
			float destX = (float)unit->GetX() - camera.GetX();
			float destY = (float)unit->GetY() - camera.GetY();

			int imgWidth = m_pShopkeeper->GetWidth();
			int maxFrame = imgWidth / NPC_SHOPKEEPER_FRAME_X;
			if (maxFrame == 0) maxFrame = 1;

			int currentFrame = (GetTickCount() / ANIM_SPEED) % maxFrame;
			float srcX = (float)(currentFrame * NPC_SHOPKEEPER_FRAME_X);
			float srcY = 0.0f;

			float drawW = (float)NPC_SHOPKEEPER_FRAME_X * NPC_SHOPKEEPER_DRAW_SCALE;
			float drawH = (float)NPC_SHOPKEEPER_FRAME_Y * NPC_SHOPKEEPER_DRAW_SCALE;

			destX += (gridSize - drawW) / 2.0f;
			destY += (gridSize - drawH);

			graphics.DrawImage(m_pShopkeeper,
				RectF(destX, destY, drawW, drawH),
				srcX, srcY, (float)NPC_SHOPKEEPER_FRAME_X, (float)NPC_SHOPKEEPER_FRAME_Y,
				UnitPixel);
		}

		float unitX = (float)unit->GetX() - camera.GetX();
		float unitY = (float)unit->GetY() - camera.GetY();

		if (unit->GetTag() == ENEMY) {
			int gridX = (int)(unit->GetX() / (FRAME_SIZE * DRAW_SCALE));
			int gridY = (int)(unit->GetY() / (FRAME_SIZE * DRAW_SCALE));
			if (Light::getInstance().GetVisibility(gridX, gridY) != VIS_VISIBLE) continue;
		}

		wchar_t szBuf[128];
		if (unit->GetTag() == PLAYER) {
			swprintf_s(szBuf, L"HP: %d/%d\nATK: %d\nDig: %d", unit->GetHp(), unit->GetMaxHp(), unit->GetAttack(), static_cast<Player*>(unit)->GetDigLevel());
		}
		else if (unit->GetTag() == NPC) {
			continue; 
		}
		else {
			swprintf_s(szBuf, L"HP: %d/%d\nATK: %d", unit->GetHp(), unit->GetMaxHp(), unit->GetAttack());
		}

		Gdiplus::PointF point(unitX, unitY - 30.0f);

		if (m_pDefaultFont && m_pWhiteBrush && m_pBlackBrush) {
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
		DrawWorldItems(graphics, map, camera);
		DrawUnit(graphics, camera);
		DrawUi(graphics, player);
		DrawRhythm(graphics);
	}

	BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteObject(memBitmap);
	DeleteDC(memDC);
	EndPaint(hWnd, &ps);
}

void Render::DrawUpdate(HWND hWnd, UnitBase& unit)
{
	InvalidateRect(hWnd, NULL, FALSE);
}

void Render::UpdateTileCache(int x, int y, Map* pMap)
{
	if (!m_pCachedBackground || !pMap) return;

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	Gdiplus::Graphics graphics(m_pCachedBackground);
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0));
	graphics.FillRectangle(&blackBrush, (float)x * gridSize, 0.0f, gridSize, (float)(MAP_HEIGHT * gridSize));

	for (int targetY = 0; targetY < MAP_HEIGHT; ++targetY) {
		MapTile tile = pMap->GetTile(x, targetY);

		if (tile.type == TILE_FLOOR) {
			graphics.DrawImage(m_pTileImg,
				RectF((float)x * gridSize, (float)targetY * gridSize, gridSize, gridSize),
				(float)TILE_DEFAULT_X, (float)TILE_DEFAULT_Y, (float)FRAME_SIZE, (float)FRAME_SIZE,
				UnitPixel);
		}
		else if (tile.type == TILE_WALL_DEFAULT) {
			graphics.DrawImage(m_pWallImg,
				RectF((float)x * gridSize, (float)targetY * gridSize - gridSize, gridSize, gridSize * 2.0f),
				(float)WALL_DEFAULT_X + (tile.variant * FRAME_SIZE), (float)WALL_DEFAULT_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
				UnitPixel);
		}
		else if (tile.type == TILE_WALL_HARD) {
			graphics.DrawImage(m_pWallImg,
				RectF((float)x * gridSize, (float)targetY * gridSize - gridSize, gridSize, gridSize * 2.0f),
				(float)WALL_HARD_X, (float)WALL_HARD_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
				UnitPixel);
		}
		else if (tile.type == TILE_WALL_BADROCK) {
			graphics.DrawImage(m_pWallImg,
				RectF((float)x * gridSize, (float)targetY * gridSize - gridSize, gridSize, gridSize * 2.0f),
				(float)WALL_BADROCK_X, (float)WALL_BADROCK_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
				UnitPixel);
		}
		else if (tile.type == TILE_WALL_SHOP) {
			graphics.DrawImage(m_pWallImg,
				RectF((float)x * gridSize, (float)targetY * gridSize - gridSize, gridSize, gridSize * 2.0f),
				(float)WALL_SHOP_X, (float)WALL_SHOP_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
				UnitPixel);
		}
	}
}

void Render::DrawTitle(HWND hWnd, Title& title)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

	{
		Gdiplus::Graphics graphics(memDC);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

		DrawTitleBackground(graphics);
		title.tRender(graphics);
	}

	BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteObject(memBitmap);
	DeleteDC(memDC);
	EndPaint(hWnd, &ps);
}

void Render::DrawTitleBackground(Gdiplus::Graphics& graphics)
{
	if (!m_pTitleImg || m_pTitleImg->GetLastStatus() != Ok) return;

	graphics.DrawImage(m_pTitleImg,
		Gdiplus::Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
		SCENE_MAINMENU_POS_X, SCENE_MAINMENU_POS_Y,
		SCENE_MAINMENU_SCR_X, SCENE_MAINMENU_SCR_Y,
		Gdiplus::UnitPixel);
}



void Render::DrawUi(Gdiplus::Graphics& graphics, UnitBase& player)
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
			(float)srcX, (float)srcY, (float)FRAME_SIZE, (float)FRAME_SIZE,
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
			(float)srcX, (float)UI_INVEN_PNG_Y, (float)UI_INVEN_FRAME_X, (float)UI_INVEN_FRAME_Y,
			UnitPixel);

		ItemSlot slot;
		switch (i) {
			case 0: slot = SLOT_SHOVEL; break;
			case 1: slot = SLOT_WEAPON; break;
			case 2: slot = SLOT_BODY; break;
			case 3: slot = SLOT_HEAD; break;
			case 4: slot = SLOT_FEET; break;
			default: slot = SLOT_TORCH; break;
		}

		ItemBase* pEquipped = static_cast<Player&>(player).GetEquippedItem(slot);
		if (pEquipped) {
			Gdiplus::Image* pAtlas = nullptr;
			switch (slot) {
				case SLOT_WEAPON: pAtlas = m_pItemWeapons; break;
				case SLOT_BODY:   pAtlas = m_pItemArmor; break;
				case SLOT_HEAD:   pAtlas = m_pItemHeadwear; break;
				case SLOT_FEET:   pAtlas = m_pItemFootwear; break;
				case SLOT_SHOVEL: pAtlas = m_pItemShovels; break;
				case SLOT_TORCH:  pAtlas = m_pItemTorches; break;
			}

			if (pAtlas && pAtlas->GetLastStatus() == Ok) {
				float iconSize = uiDrawWidth * 0.8f; 
				float iconX = destX + (uiDrawWidth - iconSize) / 2.0f;
				float iconY = UI_INVEN_POS_Y + (uiDrawHeight - iconSize) / 2.0f;

				graphics.DrawImage(pAtlas,
					RectF(iconX, iconY, iconSize, iconSize),
					(float)pEquipped->GetSrcX(), (float)pEquipped->GetSrcY(), (float)pEquipped->GetSrcW(), (float)pEquipped->GetSrcH(),
					UnitPixel);
			}
		}
	}

	for (int i = 0; i < 2; i++)
	{
		float destY = UI_ITEM_POS_Y + i * (uiDrawHeight + UI_GAP);
		float srcY = (float)(UI_ITEM_PNG_Y + i * (UI_ITEM_FRAME_PNG_Y + UI_ITEM_GAP_Y));
		graphics.DrawImage(m_HUD,
			RectF(UI_ITEM_POS_X, destY, uiDrawWidth, uiDrawHeight),
			(float)UI_ITEM_PNG_X, (float)srcY, (float)UI_ITEM_FRAME_PNG_X, (float)UI_ITEM_FRAME_PNG_Y,
			UnitPixel);
	}

	graphics.DrawImage(m_HUD,
		RectF(UI_MONEY_POS_X, UI_MONEY_POS_Y, drawSize, drawSize),
		(float)UI_MONEY_SCR_X, (float)UI_MONEY_SCR_Y, (float)FRAME_SIZE, (float)FRAME_SIZE,
		UnitPixel);

	graphics.DrawImage(m_HUD,
		RectF(UI_JEWEL_POS_X, UI_JEWEL_POS_Y, drawSize, drawSize),
		(float)UI_JEWEL_SCR_X, (float)UI_JEWEL_SCR_Y, (float)FRAME_SIZE, (float)FRAME_SIZE,
		UnitPixel);

	wchar_t szBuf[128];
	swprintf_s(szBuf, L" x%d", 0);
	DrawString(graphics, szBuf, 24, (float)UI_MONEY_FONT_POS_X, (float)UI_MONEY_FONT_POS_Y);
	DrawString(graphics, szBuf, 24, (float)UI_JEWEL_FONT_POS_X, (float)UI_JEWEL_FONT_POS_Y);
}

void Render::DrawRhythm(Gdiplus::Graphics& graphics)
{
	if (!m_HUD || m_HUD->GetLastStatus() != Ok) return;

	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	int currentFrame = RhythmManager::getInstance().GetCurrentFrame();
	int drawScale = DRAW_SCALE + 3;
	float drawSize = (float)(FRAME_SIZE * drawScale);
	float startX = UI_RHYTHM_HEART_POS_X - (drawSize / 2);

	float srcX = (float)UI_RHYTHM_HEART_SCR_X + (currentFrame * UI_RHYTHM_HEART_FRAME_X);

	graphics.DrawImage(m_HUD,
		RectF(startX, (float)UI_RHYTHM_HEART_POS_Y, drawSize, drawSize),
		srcX, (float)UI_RHYTHM_HEART_SCR_Y, (float)UI_RHYTHM_HEART_FRAME_X, (float)UI_RHYTHM_HEART_FRAME_Y,
		UnitPixel);

	R_Note* pNote = RhythmManager::getInstance().GetCurrentNote();
	if (pNote) {
		float cTime = pNote->Getcurrenttime();
		float mTime = pNote->Getmaxtime();
		float ratio = cTime / mTime;

		float beatWidth = 300.0f;
		SolidBrush whiteBrush(Color(255, 255, 255, 255));
		Pen whitePen(Color(150, 255, 255, 255), 2);


		for (int i = 1; i <= 4; ++i) {
			float dist = (i - ratio) * beatWidth;
			if (dist < 0) continue;

			float leftNoteX = (float)UI_RHYTHM_BAR_POS_X - dist;
			float rightNoteX = (float)UI_RHYTHM_BAR_POS_X + dist;

			float noteW = 8.0f;

			graphics.DrawImage(m_HUD,
				RectF(leftNoteX - noteW / 2, (float)UI_RHYTHM_BAR_POS_Y + UI_RHYTHM_BAR_FRAME_Y / 2, UI_RHYTHM_BAR_FRAME_X * DRAW_SCALE, FRAME_SIZE * UI_BAR_DRAW_SCALE),
				(float)UI_RHYTHM_BAR_SCR_X, (float)UI_RHYTHM_BAR_SCR_Y, (float)UI_RHYTHM_BAR_FRAME_X, (float)UI_RHYTHM_BAR_FRAME_Y,
				UnitPixel);
			graphics.DrawImage(m_HUD,
				RectF(rightNoteX - noteW / 2, (float)UI_RHYTHM_BAR_POS_Y + UI_RHYTHM_BAR_FRAME_Y / 2, UI_RHYTHM_BAR_FRAME_X * DRAW_SCALE, FRAME_SIZE * UI_BAR_DRAW_SCALE),
				(float)UI_RHYTHM_BAR_SCR_X, (float)UI_RHYTHM_BAR_SCR_Y, (float)UI_RHYTHM_BAR_FRAME_X, (float)UI_RHYTHM_BAR_FRAME_Y,
				UnitPixel);
		}

		float marginRatio = RHYTHM_MARGIN / mTime;
		float marginWidth = marginRatio * beatWidth;
		SolidBrush marginBrush(Color(60, 0, 255, 0));
		graphics.FillRectangle(&marginBrush, RectF((float)UI_RHYTHM_BAR_POS_X - marginWidth, (float)UI_RHYTHM_BAR_POS_Y, marginWidth * 2, 10));
	}
}

void Render::DrawWorldItems(Gdiplus::Graphics& graphics, Map& map, Camera& camera)
{
	const auto& worldItems = map.GetWorldItems();
	if (worldItems.empty()) return;

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	float camX = camera.GetX();
	float camY = camera.GetY();

	for (const auto& wi : worldItems)
	{
		if (!wi.item) continue;

		float drawX = (float)wi.x * gridSize - camX;
		float drawY = (float)wi.y * gridSize - camY;

		if (drawX + gridSize < 0 || drawX > SCREEN_WIDTH || drawY + gridSize < 0 || drawY > SCREEN_HEIGHT)
			continue;

		Gdiplus::Image* pAtlas = nullptr;
		switch (wi.item->GetSlot())
		{
		case SLOT_WEAPON:     pAtlas = m_pItemWeapons; break;
		case SLOT_BODY:       pAtlas = m_pItemArmor; break;
		case SLOT_HEAD:       pAtlas = m_pItemHeadwear; break;
		case SLOT_FEET:       pAtlas = m_pItemFootwear; break;
		case SLOT_SHOVEL:     pAtlas = m_pItemShovels; break;
		case SLOT_CONSUMABLE: pAtlas = m_pItemConsumables; break;
		default: pAtlas = m_pItemResources; break;
		}

		if (pAtlas && pAtlas->GetLastStatus() == Ok)
		{
			graphics.DrawImage(pAtlas,
				RectF(drawX, drawY, gridSize, gridSize),
				(float)wi.item->GetSrcX(), (float)wi.item->GetSrcY(), (float)wi.item->GetSrcW(), (float)wi.item->GetSrcH(),
				UnitPixel);
		}
	}
}

void Render::DrawTitleMenuOption(Gdiplus::Graphics& graphics, int nOptionIdx, bool bSelected, bool bDraw, float x, float y)
{
	const wchar_t* szMenus[] = { L"START", L"RESET", L"EXIT" };
	if (nOptionIdx < 0 || nOptionIdx > 2) return;

	Gdiplus::Font* pFont = bSelected ? m_pSelectedFont : m_pBigFont;
	if (!pFont || !m_pWhiteBrush || !m_pBlackBrush) return;

	Gdiplus::PointF point(x, y);

	// 외곽선 효과
	for (int ox = -1; ox <= 1; ++ox) {
		for (int oy = -1; oy <= 1; ++oy) {
			if (ox == 0 && oy == 0) continue;
			graphics.DrawString(szMenus[nOptionIdx], -1, pFont, Gdiplus::PointF(point.X + ox, point.Y + oy), m_pBlackBrush);
		}
	}
	graphics.DrawString(szMenus[nOptionIdx], -1, pFont, point, m_pWhiteBrush);
}
