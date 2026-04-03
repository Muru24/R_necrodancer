#include "Define.h"
#include "Render.h"
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace Gdiplus;

Render::Render() : m_pTileImg(nullptr), m_pSpriteAtlas(nullptr), m_pWallImg(nullptr),
		m_pCachedBackground(nullptr), m_bCacheDirty(true)
{
}

Render::~Render()
{
}

// GDI+ 초기화 및 게임에 필요한 외부 이미지 자산들을 로드합니다.
void Render::Initialize(ULONG_PTR& gdiplusToken)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_pSpriteAtlas = new Image(L"Sprite/Characters.png");
	if (m_pSpriteAtlas->GetLastStatus() != Ok) {
		std::cerr << "캐릭터 이미지 로드 실패! (Sprite/Characters.png)\n";
	}

	m_pTileImg = new Image(L"Sprite/Floor/Floors.png");
	if (m_pTileImg->GetLastStatus() != Ok) {
		std::cerr << "배경 이미지 로드 실패! (Sprite/Floor/Floors.png)\n";
	}

	m_pWallImg = new Image(L"Sprite/Floor/Walls.png");
	if (m_pWallImg->GetLastStatus() != Ok) {
		std::cerr << "벽 이미지 로드 실패! (Sprite/Floor/Walls.png)\n";
	}
}

// 할당된 이미지 자원 및 GDI+ 토큰을 해제합니다.
void Render::Finalize(ULONG_PTR gdiplusToken)
{
	if (m_pSpriteAtlas)    delete m_pSpriteAtlas;
	if (m_pTileImg)        delete m_pTileImg;
	if (m_pWallImg)        delete m_pWallImg;
	if (m_pCachedBackground) delete m_pCachedBackground;

	GdiplusShutdown(gdiplusToken);
}

// 맵 구역을 한 장의 비트맵으로 구워 캐싱한 뒤 화면에 출력합니다.
void Render::DrawBackground(Graphics& graphics, const RECT& rect, Map& map, Camera& camera)
{
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

	float gridSize = (float)(FRAME_SIZE * DRAW_SCALE);
	float camX = camera.GetX();
	float camY = camera.GetY();

	// 캐시가 유효하지 않거나 비트맵이 없으면 전체 지형을 다시 그립니다.
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

		// 안내용 그리드 라인도 비트맵에 미리 그려 최적화합니다.
		Pen gridPen(Color(120, 255, 255, 255), 2.0f);
		for (float x = 0; x <= (float)(MAP_WIDTH * gridSize); x += gridSize) {
			cacheG.DrawLine(&gridPen, (REAL)x, 0.0f, (REAL)x, (REAL)(MAP_HEIGHT * gridSize));
		}
		for (float y = 0; y <= (float)(MAP_HEIGHT * gridSize); y += gridSize) {
			cacheG.DrawLine(&gridPen, 0.0f, (REAL)y, (REAL)(MAP_WIDTH * gridSize), (REAL)y);
		}

		m_bCacheDirty = false;
	}

	// 캐싱된 배경 비트맵을 카메라 오프셋에 맞춰 한 번에 출력합니다.
	if (m_pCachedBackground) {
		graphics.DrawImage(m_pCachedBackground, -camX, -camY);
	}
}

// 플레이어를 월드 좌표에 맞춰 화면에 그립니다.
void Render::DrawPlayer(Graphics& graphics, Player& player, Camera& camera)
{
	if (!m_pSpriteAtlas || m_pSpriteAtlas->GetLastStatus() != Ok) return;

	int currentFrame = player.GetCurrentFrame();
	int camX = camera.GetX();
	int camY = camera.GetY();

	float playerX = (float)player.GetX() - camX;
	float playerY = (float)player.GetY() - camY;

	// 이동 중에만 점프 애니메이션 오프셋 적용
	float jumpOffsetY = 0.0f;
	if (player.GetIsMoving()) {
		float progress = player.GetMoveProgress();
		float jumpHeight = player.GetJumpHeight();
		jumpOffsetY = sinf(progress * 3.141592f) * jumpHeight;
	}

	bool isLookLeft = player.GetIsLookLeft();

	int bSrcX = currentFrame * FRAME_SIZE;
	int bSrcY = FRAME_SIZE * 2; // 몸체 스프라이트 Y 좌표
	int hSrcX = currentFrame * FRAME_SIZE;
	int hSrcY = 0;              // 머리 스프라이트 Y 좌표

	float drawWidth = (float)(FRAME_SIZE * DRAW_SCALE);
	float drawHeight = (float)(FRAME_SIZE * DRAW_SCALE);

	if (isLookLeft) {
		graphics.DrawImage(m_pSpriteAtlas,
			RectF(playerX + drawWidth, (playerY - jumpOffsetY) + (float)DRAW_SCALE, -drawWidth, drawHeight),
			(REAL)bSrcX, (REAL)bSrcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE, UnitPixel);

		graphics.DrawImage(m_pSpriteAtlas,
			RectF(playerX + drawWidth, (playerY - jumpOffsetY), -drawWidth, drawHeight),
			(REAL)hSrcX, (REAL)hSrcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE, UnitPixel);
	}
	else {
		graphics.DrawImage(m_pSpriteAtlas,
			RectF(playerX, (playerY - jumpOffsetY) + (float)DRAW_SCALE, drawWidth, drawHeight),
			(REAL)bSrcX, (REAL)bSrcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE, UnitPixel);

		graphics.DrawImage(m_pSpriteAtlas,
			RectF(playerX, (playerY - jumpOffsetY), drawWidth, drawHeight),
			(REAL)hSrcX, (REAL)hSrcY, (REAL)FRAME_SIZE, (REAL)FRAME_SIZE, UnitPixel);
	}
}

// 윈도우 핸들을 받아 더블 버퍼링 방식으로 최종 화면을 출력합니다.
void Render::Draw(HWND hWnd, Player& player, Map& map, Camera& camera)
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
		DrawPlayer(graphics, player, camera);
	}

	BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteObject(memBitmap);
	DeleteDC(memDC);
	EndPaint(hWnd, &ps);
}

// 화면 갱신을 요청합니다.
void Render::DrawUpdate(HWND hWnd, Player& player)
{
	InvalidateRect(hWnd, NULL, FALSE);
}
