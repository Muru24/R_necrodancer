#include "Skeleton.h"
#include "MainGame.h"
#include "Map.h"
#include "Player.h"
#include <queue>
#include <map>
#include <cmath>
#include "RhythmManager.h"

Skeleton::Skeleton(int hp, int attack, int movedistance, Vector2 pos, ObjectTag tag)
	: UnitBase(hp, attack, movedistance, pos, tag, MONSTER_SKELETON), m_beatCount(0)
{
}

void Skeleton::Move()
{
	if (GetIsMoving()) return;

	m_beatCount = (m_beatCount + 1) % 2;

	// 2박자에 한 번 이동 (m_beatCount가 다시 0이 될 때 이동)
	if (m_beatCount != 0) return;

	Player* pPlayer = MainGame::getInstance().GetPlayer();
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pPlayer || !pMap) return;

	int gridSize = FRAME_SIZE * DRAW_SCALE;
	Vector2 myGridPos = { floor(GetLogicalPos().X / gridSize), floor(GetLogicalPos().Y / gridSize) };
	Vector2 targetGridPos = { floor(pPlayer->GetLogicalPos().X / gridSize), floor(pPlayer->GetLogicalPos().Y / gridSize) };

	Vector2 nextStep = FindNextStepAStar(myGridPos, targetGridPos);
	
	int dx = (int)(nextStep.X - myGridPos.X);
	int dy = (int)(nextStep.Y - myGridPos.Y);

	if (dx != 0 || dy != 0) {
		TryMove(dx, dy);
	}
}

void Skeleton::Update()
{
	// UnitBase::Update()의 기본 애니메이션 로직을 대체하기 위해 직접 구현
	float deltaTime = MainGame::getInstance().GetDeltaTime();

	// 이동 처리
	if (GetIsMoving()) {
		int speed = MOVE_SPEED;
		if (m_isBumping) speed *= 2;

		float progress = GetMoveProgress() + speed * deltaTime;
		SetMoveProgress(progress);

		if (progress >= 1.0f)
		{
			SetMoveProgress(1.0f);
			if (m_isBumping) SetPos(m_vStartPos);
			else SetPos(m_vTargetPos);
			SetIsMoving(false);
			m_isBumping = false;
			SetMoveProgress(0.0f);
		}
		else
		{
			Vector2 pos;
			if (m_isBumping) {
				float bumpFactor = sinf(progress * 3.141592f) * 0.3f;
				pos.X = m_vStartPos.X + (m_vTargetPos.X - m_vStartPos.X) * bumpFactor;
				pos.Y = m_vStartPos.Y + (m_vTargetPos.Y - m_vStartPos.Y) * bumpFactor;
			} else {
				pos.X = m_vStartPos.X + (m_vTargetPos.X - m_vStartPos.X) * progress;
				pos.Y = m_vStartPos.Y + (m_vTargetPos.Y - m_vStartPos.Y) * progress;
			}
			SetPos(pos);
		}
	}

	// 8프레임 애니메이션 처리 (1번박자: 0~3, 2번박자: 4~7)
	float ratio = RhythmManager::getInstance().GetRatio();
	int baseFrame = (m_beatCount == 1) ? 0 : 4; // Move()에서 m_beatCount를 먼저 올리므로 반대로 적용
	SetCurrentFrame(baseFrame + (int)(ratio * 4) % 4);
}

struct AStarNode {
	Vector2 pos;
	float g, h;
	AStarNode* parent;

	float f() const { return g + h; }
};

struct CompareNode {
	bool operator()(AStarNode* a, AStarNode* b) {
		return a->f() > b->f();
	}
};

Vector2 Skeleton::FindNextStepAStar(Vector2 startGrid, Vector2 targetGrid)
{
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return startGrid;

	std::priority_queue<AStarNode*, std::vector<AStarNode*>, CompareNode> openSet;
	std::map<std::pair<int, int>, float> gScore;
	std::vector<AStarNode*> allNodes;

	auto startNode = new AStarNode{ startGrid, 0, abs(targetGrid.X - startGrid.X) + abs(targetGrid.Y - startGrid.Y), nullptr };
	openSet.push(startNode);
	allNodes.push_back(startNode);
	gScore[{ (int)startGrid.X, (int)startGrid.Y }] = 0;

	Vector2 dirs[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

	while (!openSet.empty()) {
		AStarNode* current = openSet.top();
		openSet.pop();

		if (current->pos.X == targetGrid.X && current->pos.Y == targetGrid.Y) {
			// 경로 찾음 -> 첫 번째 단계 반환
			AStarNode* step = current;
			while (step->parent && (step->parent->pos.X != startGrid.X || step->parent->pos.Y != startGrid.Y)) {
				step = step->parent;
			}
			Vector2 result = step->pos;
			for (auto n : allNodes) delete n;
			return result;
		}

		for (const auto& dir : dirs) {
			Vector2 neighborPos = { current->pos.X + dir.X, current->pos.Y + dir.Y };
			
			// 벽 체크
			TileType t = pMap->GetTile((int)neighborPos.X, (int)neighborPos.Y).type;
			if (t != TILE_FLOOR) continue;

			float tentativeG = current->g + 1;
			if (gScore.find({ (int)neighborPos.X, (int)neighborPos.Y }) == gScore.end() || tentativeG < gScore[{ (int)neighborPos.X, (int)neighborPos.Y }]) {
				gScore[{ (int)neighborPos.X, (int)neighborPos.Y }] = tentativeG;
				auto neighborNode = new AStarNode{ neighborPos, tentativeG, abs(targetGrid.X - neighborPos.X) + abs(targetGrid.Y - neighborPos.Y), current };
				openSet.push(neighborNode);
				allNodes.push_back(neighborNode);
			}
		}

		// 너무 오래 걸리지 않도록 제한 (최대 100노드)
		if (allNodes.size() > 100) break;
	}

	for (auto n : allNodes) delete n;
	return startGrid;
}
