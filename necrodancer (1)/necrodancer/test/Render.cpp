#include "Define.h"
#include "Render.h"
#include "UnitBase.h"
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include "ObjectContainer.h"
#include "DeadRinger.h"
#include "DeadRingerPhantom.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include "Light.h"
#include "RhythmManager.h"
#include "R_Note.h"
#include "Title.h"
#include "ResourceManager.h"
#include "MainGame.h"

using namespace Gdiplus;

Render::Render() : m_bCacheDirty(true), m_aniPhase(0)
{
	for (int i = 0; i < 3; ++i) m_pBackgrounds[i] = nullptr;
}

Render::~Render()
{
}

void Render::Initialize(ULONG_PTR& gdiplusToken)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	ResourceManager::getInstance().Init();
}

void Render::Finalize(ULONG_PTR gdiplusToken)
{
	ResourceManager::getInstance().Release();

	for (int i = 0; i < 3; ++i) {
		if (m_pBackgrounds[i]) {
			delete m_pBackgrounds[i];
			m_pBackgrounds[i] = nullptr;
		}
	}

	GdiplusShutdown(gdiplusToken);
}

void Render::DrawString(Gdiplus::Graphics& graphics, wchar_t buf[], int size, int x, int y)
{
	if (!GetBigFont() || !GetWhiteBrush() || !GetBlackBrush()) return;

	Gdiplus::PointF point((float)x, (float)y);

	for (int ox = -1; ox <= 1; ++ox) {
		for (int oy = -1; oy <= 1; ++oy) {
			if (ox == 0 && oy == 0) continue;
			graphics.DrawString(buf, -1, GetBigFont(), PointF(point.X + ox, point.Y + oy), GetBlackBrush());
		}
	}
	graphics.DrawString(buf, -1, GetBigFont(), point, GetWhiteBrush());
}

void Render::DrawBackground(Gdiplus::Graphics& graphics, const RECT& rect, Map& map, Camera& camera)
{
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);

	if (m_bCacheDirty || !m_pBackgrounds[0]) {
		for (int i = 0; i < 3; ++i) {
			if (m_pBackgrounds[i]) {
				delete m_pBackgrounds[i];
				m_pBackgrounds[i] = nullptr;
			}
			m_pBackgrounds[i] = new Gdiplus::Bitmap((int)(MAP_WIDTH * gridSize), (int)(MAP_HEIGHT * gridSize), PixelFormat32bppARGB);
			DrawBackgroundInternal(i, map);
		}
		m_bCacheDirty = false;
	}

	int targetIdx = 0;
	Player* pPlayer = static_cast<Player*>(MainGame::getInstance().GetPlayer());
	if (pPlayer && pPlayer->GetComboCount() >= 1) {
		targetIdx = m_aniPhase + 1; // 1 or 2
	}

	float camX = camera.GetX();
	float camY = camera.GetY();


	graphics.DrawImage(m_pBackgrounds[targetIdx],
		Gdiplus::RectF(0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT),
		camX, camY, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT,
		Gdiplus::UnitPixel);


	float fGridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	const auto& units = ObjectContainer::getInstance().GetUnitContainer();
	SolidBrush dangerBrush(Color(120, 255, 0, 0));

	for (auto* unit : units) {
		if (unit && unit->GetMonsterType() == MONSTER_DEAD_RINGER) {
			DeadRinger* pDR = dynamic_cast<DeadRinger*>(unit);
			DeadRingerPhantom* pDP = dynamic_cast<DeadRingerPhantom*>(unit);

			const std::vector<Vector2>* pPath = nullptr;
			if (pDR) {
				pPath = &pDR->GetSkillPath();
			} else if (pDP) {
				pPath = &pDP->GetSkillPath();
			}

			if (pPath) {
				for (const auto& gridPos : *pPath) {
					graphics.FillRectangle(&dangerBrush, 
						(float)gridPos.X * fGridSize - camX, (float)gridPos.Y * fGridSize - camY, 
						(float)fGridSize, (float)fGridSize);
				}
			}
		}
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
				SolidBrush darkBrush(Color(150, 0, 0, 0));
				graphics.FillRectangle(&darkBrush, RectF(drawX, drawY, gridSize, gridSize));
			}
		}
	}
}

void Render::DrawBackgroundInternal(int index, Map& map)
{
	if (!m_pBackgrounds[index]) return;

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	Gdiplus::Graphics cacheG(m_pBackgrounds[index]);
	cacheG.SetInterpolationMode(InterpolationModeNearestNeighbor);
	cacheG.Clear(Gdiplus::Color(255, 0, 0, 0));

	if (ResourceManager::getInstance().GetTileImg() && ResourceManager::getInstance().GetWallImg() &&
		ResourceManager::getInstance().GetTileImg()->GetLastStatus() == Ok &&
		ResourceManager::getInstance().GetWallImg()->GetLastStatus() == Ok)
	{
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				MapTile tile = map.GetTile(x, y);

				if (tile.type == TILE_FLOOR) {
					int srcX = TILE_DEFAULT_X;
					int srcY = TILE_DEFAULT_Y;

					if (index == 1) { // Phase 0 (Odd)
						if ((x + y) % 2 != 0) {
							srcX = TILE_ANI_POS_X;
							srcY = TILE_ANI_POS_Y;
						}
					}
					else if (index == 2) { // Phase 1 (Even)
						if ((x + y) % 2 == 0) {
							srcX = 52;
							srcY = 25;
						}
					}

					cacheG.DrawImage(ResourceManager::getInstance().GetTileImg(),
						RectF((float)x * gridSize, (float)y * gridSize, gridSize, gridSize),
						(float)srcX, (float)srcY, (float)TILE_SCR_X, (float)TILE_SCR_Y,
						UnitPixel);
				}
				else if (tile.type == TILE_BOSS_SPECIAL_FLOOR) {
					cacheG.DrawImage(ResourceManager::getInstance().GetTileImg(),
						RectF((float)x * gridSize, (float)y * gridSize, gridSize, gridSize),
						(float)TILE_BOSS_FLOOR_SRC_X, (float)TILE_BOSS_FLOOR_SRC_Y, (float)TILE_SCR_X, (float)TILE_SCR_Y,
						UnitPixel);
				}
				else if (tile.type == TILE_WALL_DEFAULT) {
					float srcX = (float)WALL_DEFAULT_X + (tile.variant * FRAME_SIZE);
					cacheG.DrawImage(ResourceManager::getInstance().GetWallImg(),
						RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
						srcX, (float)WALL_DEFAULT_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
						UnitPixel);
				}
				else if (tile.type == TILE_WALL_HARD) {
					cacheG.DrawImage(ResourceManager::getInstance().GetWallImg(),
						RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
						(float)WALL_HARD_X, (float)WALL_HARD_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
						UnitPixel);
				}
				else if (tile.type == TILE_WALL_BADROCK) {
					cacheG.DrawImage(ResourceManager::getInstance().GetWallImg(),
						RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
						(float)WALL_BADROCK_X, (float)WALL_BADROCK_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
						UnitPixel);
				}
				else if (tile.type == TILE_WALL_SHOP) {
					cacheG.DrawImage(ResourceManager::getInstance().GetWallImg(),
						RectF((float)x * gridSize, (float)y * gridSize - gridSize, gridSize, gridSize * 2.0f),
						(float)WALL_SHOP_X, (float)WALL_SHOP_Y, (float)FRAME_SIZE, FRAME_SIZE + (FRAME_SIZE * 0.5f),
						UnitPixel);
				}
			}
		}
	}
}


void Render::DrawPlayer(Graphics& graphics, ::UnitBase& unit, Camera& camera)
{
	if (!ResourceManager::getInstance().GetSpriteAtlas() || ResourceManager::getInstance().GetSpriteAtlas()->GetLastStatus() != Ok) return;

	int currentFrame = unit.GetCurrentFrame();

	int bSrcX = currentFrame * FRAME_SIZE;
	int bSrcY = FRAME_SIZE * 2;
	int hSrcX = currentFrame * FRAME_SIZE;
	int hSrcY = 0;

	DrawUnitInternal(graphics, ResourceManager::getInstance().GetSpriteAtlas(), unit, camera, bSrcX, bSrcY, (float)DRAW_SCALE);
	DrawUnitInternal(graphics, ResourceManager::getInstance().GetSpriteAtlas(), unit, camera, hSrcX, hSrcY, 0.0f);
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
			MonsterType mType = unit->GetMonsterType();
			Gdiplus::Image* pImg = nullptr;
			float srcX = 0, srcY = 0, srcW = 0, srcH = 0;

			if (mType == MONSTER_SLIME) {
				pImg = ResourceManager::getInstance().GetSlimeImg();
				srcW = (float)MONSTER_SLIME_SCR_X;
				srcH = (float)MONSTER_SLIME_SCR_Y;
				srcX = (float)MONSTER_SLIME_POS_X + (currentFrame * srcW);
				srcY = (float)MONSTER_SLIME_POS_Y;
			}
			else if (mType == MONSTER_BAT) {
				pImg = ResourceManager::getInstance().GetBatImg();
				srcW = (float)MONSTER_BAT_SCR_X;
				srcH = (float)MONSTER_BAT_SCR_Y;
				srcX = (float)MONSTER_BAT_POS_X + (currentFrame * srcW);
				srcY = (float)MONSTER_BAT_POS_Y;
			}
			else if (mType == MONSTER_SKELETON) {
				pImg = ResourceManager::getInstance().GetSkeletonImg();
				srcW = (float)MONSTER_SKELETON_SCR_X;
				srcH = (float)MONSTER_SKELETON_SCR_Y;
				srcX = (float)MONSTER_SKELETON_POS_X + (currentFrame * srcW);
				srcY = (float)MONSTER_SKELETON_POS_Y;
			}
			else if (mType == MONSTER_DEAD_RINGER) {
				pImg = ResourceManager::getInstance().GetDeadRingerImg();
				DeadRinger* pDR = dynamic_cast<DeadRinger*>(unit);
				DeadRingerPhantom* pDP = dynamic_cast<DeadRingerPhantom*>(unit);
				
				if (pDR && pDR->IsPhase2()) {

					srcW = (float)MONSTER_DEAD_RINGER_SKILL2_SCR_X;
					srcH = (float)MONSTER_DEAD_RINGER_SKILL2_SCR_Y;
					srcX = (float)MONSTER_DEAD_RINGER_SKILL2_POS_X + (currentFrame * srcW);
					srcY = (float)MONSTER_DEAD_RINGER_SKILL2_POS_Y;
				}
				else if (pDP || (pDR && (pDR->GetState() == DeadRingerState::READY || 
							pDR->GetState() == DeadRingerState::DASH || 
							pDR->GetState() == DeadRingerState::DASHING))) {

					srcW = (float)MONSTER_DEAD_RINGER_SKILL1_SCR_X;
					srcH = (float)MONSTER_DEAD_RINGER_SKILL1_SCR_Y;
					srcX = (float)MONSTER_DEAD_RINGER_SKILL1_POS_X + (currentFrame * srcW);
					srcY = (float)MONSTER_DEAD_RINGER_SKILL1_POS_Y;
				}
				else {

					srcW = (float)MONSTER_DEAD_RINGER_SCR_X;
					srcH = (float)MONSTER_DEAD_RINGER_SCR_Y;
					srcX = (float)MONSTER_DEAD_RINGER_POS_X + (currentFrame * srcW);
					srcY = (float)MONSTER_DEAD_RINGER_POS_Y;
				}
			}

			if (!pImg || pImg->GetLastStatus() != Ok) continue;

			float unitX = (float)unit->GetX() - camera.GetX();
			float unitY = (float)unit->GetY() - camera.GetY();

			float jumpOffsetY = 0.0f;
			if (unit->GetIsMoving()) {
				float progress = unit->GetMoveProgress();
				float jumpHeight = unit->GetJumpHeight();
				jumpOffsetY = sinf(progress * 3.141592f) * jumpHeight;
			}

			bool isLookLeft = unit->GetIsLookLeft();
			float drawWidth = (float)(srcW * DRAW_SCALE);
			float drawHeight = (float)(srcH * DRAW_SCALE);

			float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
			float offsetX = (gridSize - drawWidth) / 2.0f;
			float offsetY = (gridSize - drawHeight);

			if (isLookLeft) {
				graphics.DrawImage(pImg,
					RectF(unitX + offsetX + drawWidth, (unitY - jumpOffsetY) + offsetY, -drawWidth, drawHeight),
					srcX, srcY, srcW, srcH, UnitPixel);
			}
			else {
				graphics.DrawImage(pImg,
					RectF(unitX + offsetX, (unitY - jumpOffsetY) + offsetY, drawWidth, drawHeight),
					srcX, srcY, srcW, srcH, UnitPixel);
			}
		}
		else if (unit->GetTag() == NPC)
		{
			int gridX = (int)(unit->GetX() / (FRAME_SIZE * DRAW_SCALE));
			int gridY = (int)(unit->GetY() / (FRAME_SIZE * DRAW_SCALE));
			if (Light::getInstance().GetVisibility(gridX, gridY) != VIS_VISIBLE) continue;
			if (!ResourceManager::getInstance().GetShopkeeper() || ResourceManager::getInstance().GetShopkeeper()->GetLastStatus() != Ok) continue;

			float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
			float destX = (float)unit->GetX() - camera.GetX();
			float destY = (float)unit->GetY() - camera.GetY();

			int imgWidth = ResourceManager::getInstance().GetShopkeeper()->GetWidth();
			int maxFrame = imgWidth / NPC_SHOPKEEPER_FRAME_X;
			if (maxFrame == 0) maxFrame = 1;

			int currentFrame = (GetTickCount() / ANIM_SPEED) % maxFrame;
			float srcX = (float)(currentFrame * NPC_SHOPKEEPER_FRAME_X);
			float srcY = 0.0f;

			float drawW = (float)NPC_SHOPKEEPER_FRAME_X * NPC_SHOPKEEPER_DRAW_SCALE;
			float drawH = (float)NPC_SHOPKEEPER_FRAME_Y * NPC_SHOPKEEPER_DRAW_SCALE;

			destX += (gridSize - drawW) / 2.0f;
			destY += (gridSize - drawH);

			graphics.DrawImage(ResourceManager::getInstance().GetShopkeeper(),
				RectF(destX, destY, drawW, drawH),
				srcX, srcY, (float)NPC_SHOPKEEPER_FRAME_X, (float)NPC_SHOPKEEPER_FRAME_Y,
				UnitPixel);
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

		for (auto* effect : ObjectContainer::getInstance().GetAttackEffects())
		{
			if (effect) DrawAttackEffect(graphics, *effect, camera);
		}

		DrawBombs(graphics, camera);
		DrawExplosions(graphics, camera);

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
	if (!ResourceManager::getInstance().GetTitleImg() || ResourceManager::getInstance().GetTitleImg()->GetLastStatus() != Ok) return;

	graphics.DrawImage(ResourceManager::getInstance().GetTitleImg(),
		Gdiplus::Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
		SCENE_MAINMENU_POS_X, SCENE_MAINMENU_POS_Y,
		SCENE_MAINMENU_SCR_X, SCENE_MAINMENU_SCR_Y,
		Gdiplus::UnitPixel);
}



void Render::DrawUi(Gdiplus::Graphics& graphics, UnitBase& player)
{
	if (!ResourceManager::getInstance().GetHUD() || ResourceManager::getInstance().GetHUD()->GetLastStatus() != Ok) return;

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

		graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
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

		graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
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
				case SLOT_WEAPON: pAtlas = ResourceManager::getInstance().GetItemWeapons(); break;
				case SLOT_BODY:   pAtlas = ResourceManager::getInstance().GetItemArmor(); break;
				case SLOT_HEAD:   pAtlas = ResourceManager::getInstance().GetItemHeadwear(); break;
				case SLOT_FEET:   pAtlas = ResourceManager::getInstance().GetItemFootwear(); break;
				case SLOT_SHOVEL: pAtlas = ResourceManager::getInstance().GetItemShovels(); break;
				case SLOT_TORCH:  pAtlas = ResourceManager::getInstance().GetItemTorches(); break;
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
		float destX = UI_ITEM_POS_X;
		float destY = UI_ITEM_POS_Y + i * (uiDrawHeight + UI_GAP);
		float srcY = (float)(UI_ITEM_PNG_Y + i * (UI_ITEM_FRAME_PNG_Y + UI_ITEM_GAP_Y));
		graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
			RectF(destX, destY, uiDrawWidth, uiDrawHeight),
			(float)UI_ITEM_PNG_X, (float)srcY, (float)UI_ITEM_FRAME_PNG_X, (float)UI_ITEM_FRAME_PNG_Y,
			UnitPixel);


		if (i == 0) {
			ItemBase* pItem = player.GetEquippedItem(SLOT_CONSUMABLE);
			if (pItem) {
				Gdiplus::Image* pAtlas = ResourceManager::getInstance().GetItemConsumables();
				if (pAtlas && pAtlas->GetLastStatus() == Ok) {
					float iconSize = uiDrawWidth * 0.8f;
					float iconX = destX + (uiDrawWidth - iconSize) / 2.0f;
					float iconY = destY + (uiDrawHeight - iconSize) / 2.0f;

					graphics.DrawImage(pAtlas,
						RectF(iconX, iconY, iconSize, iconSize),
						(float)pItem->GetSrcX(), (float)pItem->GetSrcY(), (float)pItem->GetSrcW(), (float)pItem->GetSrcH(),
						UnitPixel);
				}
			}
		}
	}

	graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
		RectF(UI_MONEY_POS_X, UI_MONEY_POS_Y, drawSize, drawSize),
		(float)UI_MONEY_SCR_X, (float)UI_MONEY_SCR_Y, (float)FRAME_SIZE, (float)FRAME_SIZE,
		UnitPixel);

	if (ResourceManager::getInstance().GetItemConsumables() && ResourceManager::getInstance().GetItemConsumables()->GetLastStatus() == Gdiplus::Ok) {
		graphics.DrawImage(ResourceManager::getInstance().GetItemConsumables(),
			RectF(UI_JEWEL_POS_X, UI_JEWEL_POS_Y, drawSize, drawSize),
			(float)ITEM_BOMB_SCR_X, (float)ITEM_BOMB_SCR_Y, (float)ITEM_BOMB_FRAME, (float)ITEM_BOMB_FRAME,
			UnitPixel);
	}

	Player& p = static_cast<Player&>(player);
	wchar_t szBuf[128];
	swprintf_s(szBuf, L" x%d", p.GetMoney());
	DrawString(graphics, szBuf, 24, (float)UI_MONEY_FONT_POS_X, (float)UI_MONEY_FONT_POS_Y);
	
	swprintf_s(szBuf, L" x%d", p.GetBombs());
	DrawString(graphics, szBuf, 24, (float)UI_JEWEL_FONT_POS_X, (float)UI_JEWEL_FONT_POS_Y);
}

void Render::DrawRhythm(Gdiplus::Graphics& graphics)
{
	if (!ResourceManager::getInstance().GetHUD() || ResourceManager::getInstance().GetHUD()->GetLastStatus() != Ok) return;

	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	int currentFrame = RhythmManager::getInstance().GetCurrentFrame();
	int drawScale = DRAW_SCALE + 3;
	float drawSize = (float)(FRAME_SIZE * drawScale);
	float startX = UI_RHYTHM_HEART_POS_X - (drawSize / 2);

	float srcX = (float)UI_RHYTHM_HEART_SCR_X + (currentFrame * UI_RHYTHM_HEART_FRAME_X);

	graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
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

			graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
				RectF(leftNoteX - noteW / 2, (float)UI_RHYTHM_BAR_POS_Y + UI_RHYTHM_BAR_FRAME_Y / 2, UI_RHYTHM_BAR_FRAME_X * DRAW_SCALE, FRAME_SIZE * UI_BAR_DRAW_SCALE),
				(float)UI_RHYTHM_BAR_SCR_X, (float)UI_RHYTHM_BAR_SCR_Y, (float)UI_RHYTHM_BAR_FRAME_X, (float)UI_RHYTHM_BAR_FRAME_Y,
				UnitPixel);
			graphics.DrawImage(ResourceManager::getInstance().GetHUD(),
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

void Render::DrawAttackEffect(Gdiplus::Graphics& graphics, AttackEffect& effect, Camera& camera)
{
	if (!ResourceManager::getInstance().GetEffectAttack() || ResourceManager::getInstance().GetEffectAttack()->GetLastStatus() != Ok) return;

	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	float worldX = (float)effect.pos.X - camera.GetX();
	float worldY = (float)effect.pos.Y - camera.GetY();

	float srcX = 0, srcY = 0, srcW = 0, srcH = 0;
	float angle = effect.angle;

	ItemID id = effect.id;
	if (id == ITEM_DAGGER) {
		srcY = (float)EFFECT_DAGGER_POS_Y;
		srcW = (float)EFFECT_DAGGER_SCR_X;
		srcH = (float)EFFECT_DAGGER_SCR_Y;
		srcX = (float)EFFECT_DAGGER_POS_X;
	}
	else if (id == ITEM_LONGSWORD) {
		srcY = (float)EFFECT_LONGSWORD_POS_Y;
		srcW = (float)EFFECT_LONGSWORD_SCR_X;
		srcH = (float)EFFECT_LONGSWORD_SCR_Y;
		srcX = (float)EFFECT_LONGSWORD_POS_X;
	}
	else if (id == ITEM_RAPIER) {
		srcY = (float)EFFECT_RAPIER_POS_Y;
		srcW = (float)EFFECT_RAPIER_SCR_X;
		srcH = (float)EFFECT_RAPIER_SCR_Y;
		srcX = (float)EFFECT_RAPIER1_POS_X; 
	}

	float progress = effect.timer / effect.duration;
	if (progress > 1.0f) progress = 1.0f;
	int frameIndex = (int)(progress * effect.maxFrames);
	if (frameIndex >= effect.maxFrames) frameIndex = effect.maxFrames - 1;

	srcX += (frameIndex * srcW);

	float drawW = srcW * DRAW_SCALE;
	float drawH = srcH * DRAW_SCALE;

	Gdiplus::GraphicsState state = graphics.Save();
	
	graphics.TranslateTransform(worldX + gridSize / 2.0f, worldY + gridSize / 2.0f);
	graphics.RotateTransform(angle);
	
	graphics.DrawImage(ResourceManager::getInstance().GetEffectAttack(),
		RectF(-drawW / 2.0f, -drawH / 2.0f, drawW, drawH),
		srcX, srcY, srcW, srcH, UnitPixel);

	graphics.Restore(state);
}

void Render::DrawBombs(Gdiplus::Graphics& graphics, Camera& camera)
{
	const std::vector<Bomb*>& bombs = ObjectContainer::getInstance().GetBombs();
	if (bombs.empty()) return;

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	Gdiplus::Image* pAtlas = ResourceManager::getInstance().GetItemConsumables();
	if (!pAtlas || pAtlas->GetLastStatus() != Ok) return;

	for (auto* pBomb : bombs)
	{
		float drawX = pBomb->pos.X - camera.GetX();
		float drawY = pBomb->pos.Y - camera.GetY();

		if (drawX + gridSize < 0 || drawX > SCREEN_WIDTH || drawY + gridSize < 0 || drawY > SCREEN_HEIGHT)
			continue;


		int frameIndex = 5 - pBomb->beatsRemaining;
		if (frameIndex < 0) frameIndex = 0;
		if (frameIndex > 4) frameIndex = 4;


		float srcX = (float)ITEM_BOMB_SCR_X + (float)ITEM_BOMB_FRAME + (frameIndex * ITEM_BOMB_FRAME);
		float srcY = (float)ITEM_BOMB_SCR_Y;

		graphics.DrawImage(pAtlas,
			RectF(drawX, drawY, gridSize, gridSize),
			srcX, srcY, (float)ITEM_BOMB_FRAME, (float)ITEM_BOMB_FRAME,
			UnitPixel);
	}
}

void Render::DrawExplosions(Gdiplus::Graphics& graphics, Camera& camera)
{
	const std::vector<ExplosionEffect*>& explosions = ObjectContainer::getInstance().GetExplosions();
	if (explosions.empty()) return;

	Gdiplus::Image* pAtlas = ResourceManager::getInstance().GetEffectAttack();
	if (!pAtlas || pAtlas->GetLastStatus() != Ok) return;

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);

	for (auto* pExp : explosions)
	{
		float drawX = pExp->pos.X - camera.GetX();
		float drawY = pExp->pos.Y - camera.GetY();

		if (drawX + gridSize < 0 || drawX > SCREEN_WIDTH || drawY + gridSize < 0 || drawY > SCREEN_HEIGHT)
			continue;

		float progress = pExp->timer / pExp->duration;
		if (progress > 1.0f) progress = 1.0f;
		int frameIndex = (int)(progress * pExp->maxFrames);
		if (frameIndex >= pExp->maxFrames) frameIndex = pExp->maxFrames - 1;

		float srcW = (float)EFFECT_BOMB_SCR_X;
		float srcH = (float)EFFECT_BOMB_SCR_Y;
		float srcX = (float)EFFECT_BOMB_POS_X + (frameIndex * srcW);
		float srcY = (float)EFFECT_BOMB_POS_Y;



		graphics.DrawImage(pAtlas,
			RectF(drawX, drawY, gridSize, gridSize),
			srcX, srcY, srcW, srcH,
			UnitPixel);
	}
}

void Render::UpdateTileCache(int x, int y, Map* pMap)
{
	if (!m_pBackgrounds[0] || !pMap) return;
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return;

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	
	for (int i = 0; i < 3; ++i) {
		Gdiplus::Graphics cacheG(m_pBackgrounds[i]);
		cacheG.SetInterpolationMode(InterpolationModeNearestNeighbor);


		RectF tileRect((float)x * gridSize, (float)y * gridSize, gridSize, gridSize);
		SolidBrush blackBrush(Color(255, 0, 0, 0));
		cacheG.FillRectangle(&blackBrush, tileRect);


		MapTile tile = pMap->GetTile(x, y);
		if (tile.type == TILE_FLOOR) {
			int srcX = TILE_DEFAULT_X;
			int srcY = TILE_DEFAULT_Y;

			if (i == 1) { // Phase 0 (Odd)
				if ((x + y) % 2 != 0) {
					srcX = TILE_ANI_POS_X;
					srcY = TILE_ANI_POS_Y;
				}
			}
			else if (i == 2) { // Phase 1 (Even)
				if ((x + y) % 2 == 0) {
					srcX = 52;
					srcY = 25;
				}
			}

			cacheG.DrawImage(ResourceManager::getInstance().GetTileImg(),
				tileRect,
				(float)srcX, (float)srcY, (float)TILE_SCR_X, (float)TILE_SCR_Y,
				UnitPixel);
		}
		else if (tile.type == TILE_BOSS_SPECIAL_FLOOR) {
			cacheG.DrawImage(ResourceManager::getInstance().GetTileImg(),
				tileRect,
				(float)TILE_BOSS_FLOOR_SRC_X, (float)TILE_BOSS_FLOOR_SRC_Y, (float)TILE_SCR_X, (float)TILE_SCR_Y,
				UnitPixel);
		}
		else {
			float srcX = 0, srcY = 0, srcW = (float)FRAME_SIZE, srcH = FRAME_SIZE + (FRAME_SIZE * 0.5f);
			Gdiplus::Image* pImg = ResourceManager::getInstance().GetWallImg();

			if (tile.type == TILE_WALL_DEFAULT) {
				srcX = (float)WALL_DEFAULT_X + (tile.variant * FRAME_SIZE);
				srcY = (float)WALL_DEFAULT_Y;
			}
			else if (tile.type == TILE_WALL_HARD) {
				srcX = (float)WALL_HARD_X;
				srcY = (float)WALL_HARD_Y;
			}
			else if (tile.type == TILE_WALL_BADROCK) {
				srcX = (float)WALL_BADROCK_X;
				srcY = (float)WALL_BADROCK_Y;
			}
			else if (tile.type == TILE_WALL_SHOP) {
				srcX = (float)WALL_SHOP_X;
				srcY = (float)WALL_SHOP_Y;
			}

			if (srcW > 0) {

				cacheG.DrawImage(pImg,
					tileRect,
					srcX, srcY + (FRAME_SIZE * 0.5f), (float)FRAME_SIZE, (float)FRAME_SIZE,
					UnitPixel);
			}
		}


		if (y + 1 < MAP_HEIGHT) {
			MapTile tileBelow = pMap->GetTile(x, y + 1);
			float srcX = -1, srcY = 0;
			if (tileBelow.type == TILE_WALL_DEFAULT) {
				srcX = (float)WALL_DEFAULT_X + (tileBelow.variant * FRAME_SIZE);
				srcY = (float)WALL_DEFAULT_Y;
			}
			else if (tileBelow.type == TILE_WALL_HARD) {
				srcX = (float)WALL_HARD_X;
				srcY = (float)WALL_HARD_Y;
			}
			else if (tileBelow.type == TILE_WALL_BADROCK) {
				srcX = (float)WALL_BADROCK_X;
				srcY = (float)WALL_BADROCK_Y;
			}
			else if (tileBelow.type == TILE_WALL_SHOP) {
				srcX = (float)WALL_SHOP_X;
				srcY = (float)WALL_SHOP_Y;
			}

			if (srcX >= 0) {
				cacheG.DrawImage(ResourceManager::getInstance().GetWallImg(),
					tileRect,
					srcX, srcY, (float)FRAME_SIZE, (float)FRAME_SIZE,
					UnitPixel);
			}
		}
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

		if (Light::getInstance().GetVisibility(wi.x, wi.y) != VIS_VISIBLE)
			continue;

		float drawX = (float)wi.x * gridSize - camX;
		float drawY = (float)wi.y * gridSize - camY;

		if (drawX + gridSize < 0 || drawX > SCREEN_WIDTH || drawY + gridSize < 0 || drawY > SCREEN_HEIGHT)
			continue;

		Gdiplus::Image* pAtlas = nullptr;
		switch (wi.item->GetSlot())
		{
		case SLOT_WEAPON:     pAtlas = ResourceManager::getInstance().GetItemWeapons(); break;
		case SLOT_BODY:       pAtlas = ResourceManager::getInstance().GetItemArmor(); break;
		case SLOT_HEAD:       pAtlas = ResourceManager::getInstance().GetItemHeadwear(); break;
		case SLOT_FEET:       pAtlas = ResourceManager::getInstance().GetItemFootwear(); break;
		case SLOT_SHOVEL:     pAtlas = ResourceManager::getInstance().GetItemShovels(); break;
		case SLOT_CONSUMABLE: pAtlas = ResourceManager::getInstance().GetItemConsumables(); break;
		default: pAtlas = ResourceManager::getInstance().GetItemResources(); break;
		}

		if (pAtlas && pAtlas->GetLastStatus() == Ok)
		{
			graphics.DrawImage(pAtlas,
				RectF(drawX, drawY, gridSize, gridSize),
				(float)wi.item->GetSrcX(), (float)wi.item->GetSrcY(), (float)wi.item->GetSrcW(), (float)wi.item->GetSrcH(),
				UnitPixel);
		}

		if (wi.item->GetPrice() > 0) {
			wchar_t szPrice[32];
			swprintf_s(szPrice, L"%dG", wi.item->GetPrice());

			Gdiplus::PointF point(drawX + (gridSize * 0.2f), drawY + gridSize);
			for (int ox = -1; ox <= 1; ++ox) {
				for (int oy = -1; oy <= 1; ++oy) {
					if (ox == 0 && oy == 0) continue;
					graphics.DrawString(szPrice, -1, GetDefaultFont(), Gdiplus::PointF(point.X + ox, point.Y + oy), GetBlackBrush());
				}
			}
			graphics.DrawString(szPrice, -1, GetDefaultFont(), point, GetWhiteBrush());
		}
	}
}

void Render::DrawTitleMenuOption(Gdiplus::Graphics& graphics, int nOptionIdx, bool bSelected, bool bDraw, float x, float y)
{
	const wchar_t* szMenus[] = { L"START", L"RESET", L"EXIT" };
	if (nOptionIdx < 0 || nOptionIdx > 2) return;

	Gdiplus::Font* pFont = bSelected ? GetSelectedFont() : GetBigFont();
	if (!pFont || !GetWhiteBrush() || !GetBlackBrush()) return;

	Gdiplus::PointF point(x, y);

	for (int ox = -1; ox <= 1; ++ox) {
		for (int oy = -1; oy <= 1; ++oy) {
			if (ox == 0 && oy == 0) continue;
			graphics.DrawString(szMenus[nOptionIdx], -1, pFont, Gdiplus::PointF(point.X + ox, point.Y + oy), GetBlackBrush());
		}
	}
	graphics.DrawString(szMenus[nOptionIdx], -1, pFont, point, GetWhiteBrush());
}

Gdiplus::Font* Render::GetDefaultFont()
{
	return ResourceManager::getInstance().GetDefaultFont();
}

Gdiplus::Font* Render::GetBigFont()
{
	return ResourceManager::getInstance().GetBigFont();
}

Gdiplus::Font* Render::GetSelectedFont()
{
	return ResourceManager::getInstance().GetSelectedFont();
}

Gdiplus::SolidBrush* Render::GetWhiteBrush()
{
	return ResourceManager::getInstance().GetWhiteBrush();
}

Gdiplus::SolidBrush* Render::GetBlackBrush()
{
	return ResourceManager::getInstance().GetBlackBrush();
}
