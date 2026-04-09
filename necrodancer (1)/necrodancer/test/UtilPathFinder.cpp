#include "UtilPathFinder.h"
#include "MainGame.h"
#include "Map.h"
#include <queue>
#include <map>
#include <cmath>

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

Vector2 UtilPathFinder::FindNextStepAStar(Vector2 startGrid, Vector2 targetGrid)
{
	Map* pMap = MainGame::getInstance().GetMap();
	if (!pMap) return startGrid;


	if ((int)startGrid.X == (int)targetGrid.X && (int)startGrid.Y == (int)targetGrid.Y)
		return startGrid;

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

		if ((int)current->pos.X == (int)targetGrid.X && (int)current->pos.Y == (int)targetGrid.Y) {

			AStarNode* step = current;
			while (step->parent && ((int)step->parent->pos.X != (int)startGrid.X || (int)step->parent->pos.Y != (int)startGrid.Y)) {
				step = step->parent;
			}
			Vector2 result = step->pos;
			for (auto n : allNodes) delete n;
			return result;
		}

		for (const auto& dir : dirs) {
			Vector2 neighborPos = { current->pos.X + dir.X, current->pos.Y + dir.Y };
			

			TileType t = pMap->GetTile((int)neighborPos.X, (int)neighborPos.Y).type;
			if (t != TILE_FLOOR && t != TILE_BOSS_SPECIAL_FLOOR) continue;

			float tentativeG = current->g + 1;
			std::pair<int, int> posPair = { (int)neighborPos.X, (int)neighborPos.Y };

			if (gScore.find(posPair) == gScore.end() || tentativeG < gScore[posPair]) {
				gScore[posPair] = tentativeG;
				auto neighborNode = new AStarNode{ neighborPos, tentativeG, abs(targetGrid.X - neighborPos.X) + abs(targetGrid.Y - neighborPos.Y), current };
				openSet.push(neighborNode);
				allNodes.push_back(neighborNode);
			}
		}


		if (allNodes.size() > 150) break;
	}

	for (auto n : allNodes) delete n;
	return startGrid;
}
