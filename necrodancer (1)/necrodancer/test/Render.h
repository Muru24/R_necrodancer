#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gdiplus.h>

class Player;
class Map;
class Camera;

// GDI+를 활용한 게임 화면 렌더링 관리 클래스 (싱글톤)
class Render
{
private:
	Render();
	~Render();

	Gdiplus::Image* m_pTileImg;       // 바닥 타일 텍스처
	Gdiplus::Image* m_pSpriteAtlas;   // 캐릭터 스프라이트 아틀라스
	Gdiplus::Image* m_pWallImg;       // 벽 타일 텍스처
	
	Gdiplus::Bitmap* m_pCachedBackground; // 미리 렌더링된 배경 비트맵 (최적화용)
	bool m_bCacheDirty;                  // 배경 캐시 갱신 필요 여부

public:
	static Render& getInstance()
	{
		static Render instance;
		return instance;
	}

	// GDI+ 시작 및 이미지 자원을 로드합니다.
	// - gdiplusToken: GDI+ 초기화 토큰
	void Initialize(ULONG_PTR& gdiplusToken);

	// GDI+ 종료 및 이미지 자원을 해제합니다.
	// - gdiplusToken: GDI+ 초기화 토큰
	void Finalize(ULONG_PTR gdiplusToken);

	// 전체 프레임을 화면에 그립니다.
	// - hWnd: 출력 대상 윈도우 핸들
	// - player: 플레이어 객체 참조
	// - map: 맵 객체 참조
	// - camera: 카메라 객체 참조
	void Draw(HWND hWnd, class Player& player, Map& map, Camera& camera);

	// 지형과 배경을 그립니다. (캐싱 기반 최적화 적용)
	// - graphics: GDI+ 그래픽스 객체
	// - rect: 화면 클라이언트 영역 크기
	// - map: 맵 데이터 객체
	// - camera: 월드 뷰 카메라
	void DrawBackground(Gdiplus::Graphics& graphics, const RECT& rect, Map& map, Camera& camera);

	// 플레이어 캐릭터와 애니메이션을 그립니다.
	// - graphics: GDI+ 그래픽스 객체
	// - player: 플레이어 데이터
	// - camera: 월드 뷰 카메라
	void DrawPlayer(Gdiplus::Graphics& graphics, class Player& player, Camera& camera);

	// (미사용 또는 확장용) 플레이어 상태 업데이트와 연동된 그리기 기능을 수행합니다.
	void DrawUpdate(HWND hWnd, class Player& player);

	// 배경 지형이 변경되었을 때 캐시를 무효화하여 다시 그리게 합니다.
	void InvalidateBackgroundCache() { m_bCacheDirty = true; }
};
