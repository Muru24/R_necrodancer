#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

// 시스템 설정 및 상수 정의
#define FRAME_SIZE          24      // 타일 및 프레임 기본 크기
#define DRAW_SCALE          3       // 화면 출력 배율
#define ANIM_SPEED          150     // 애니메이션 재생 속도 (ms)
#define MOVE_SPEED          10      // 플레이어 이동 속도

#define SCREEN_WIDTH        1200    // 화면 너비
#define SCREEN_HEIGHT       800     // 화면 높이

#define MAP_WIDTH           60      // 맵 가로 타일 수
#define MAP_HEIGHT          40      // 맵 세로 타일 수

// 타일 및 벽 아틀라스 좌표
#define Tile_DEFAULT_X      48
#define Tile_DEFAULT_Y      0

#define WALL_DEFAULT_X      0
#define WALL_DEFAULT_Y      0
#define WALL_DEFAULT_RANGE  15      // 기본 벽의 랜덤 변형 범위
#define WALL_HARD_X         696
#define WALL_HARD_Y         0
#define WALL_BADROCK_X      120
#define WALL_BADROCK_Y      288
#define WALL_SHOP_X         0
#define WALL_SHOP_Y         288

// 맵 타일 유형 정의
enum TileType
{
    TILE_EMPTY = 0,         // 빈 공간
    TILE_WALL_DEFULT,       // 기본 벽 (1티어 흙벽)
    TILE_WALL_HARD,         // 단단한 벽 (2티어 돌벽)
    TILE_WALL_BADROCK,      // 파괴 불가능한 벽 (베드락)
    TILE_WALL_SHOP,         // 상점 벽
    TILE_FLOOR              // 바닥
};
