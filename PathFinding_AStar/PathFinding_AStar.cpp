#include <iostream>
#include <vector>
#include "olcConsoleGameEngine.h"

using namespace std;

class PathFinding : public olcConsoleGameEngine
{
public:
	PathFinding()
	{
		m_sAppName = L"Path Finding";
	}
private:


	// make node 
	struct Node
	{
		bool bObstacle = false;
		bool bVisited = false;
		float fGlobalGoal;
		float fLocalGoal;
		int _x;
		int _y;
		vector<Node*> vecNeighbours;
		Node* parent;
	};

	Node* nodeStart = nullptr;
	Node* nodeEnd = nullptr;

	// make grid
	Node* nodes = nullptr;
	int nMapWidth = 10;
	int nMapHeight = 10;

protected:
	virtual bool OnUserCreate()
	{
		nodes = new Node[nMapWidth * nMapHeight];

		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				// index 공식
				//세로 * 맵 길이 + 가로 -> 세로는 픽셀의 세로 줄이고, 가로는 1 픽셀이라고 생각하면 됨
				int index = y * nMapWidth + x;
				nodes[index]._x = x;
				nodes[index]._y = y;
				nodes[index].bObstacle = false;
				nodes[index].parent = nullptr;
				nodes[index].bVisited = false;
			}
		}
		// Create connections 
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				if (y > 0)
				{
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 0)]);
				}
				if (y < nMapWidth - 1)
				{
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 0)]);
				}
				if (x > 0)
				{
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x - 1)]);
				}
				if (x < nMapWidth - 1)
				{
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x + 1)]);
				}
		// We can also connect diagonally
				//if (y > 0 && x > 0)
				//	nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x - 1)]);
				//if (y < nMapHeight - 1 && x>0)
				//	nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x - 1)]);
				//if (y > 0 && x < nMapWidth - 1)
				//	nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 1)]);
				//if (y < nMapHeight - 1 && x < nMapWidth - 1)
				//	nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 1)]);
			}
		}

/*

*/

		nodeStart = &nodes[(nMapHeight / 2) * nMapWidth + 1];
		nodeEnd = &nodes[(nMapHeight / 2) * nMapWidth + nMapWidth - 2];


		return true;
	}

	void Solve_AStar()
	{
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].bVisited = false;
				nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;
				nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
				nodes[y * nMapWidth + x].parent = nullptr;
			}
		}

		auto distance = [](Node* a, Node* b)
			{
				return sqrtf((a->_x - b->_x) * (a->_x - b->_x) + (a->_y - b->_y) * (a->_y - b->_y));
			};

		auto heuristic = [distance](Node* a, Node* b)
			{
				return distance(a, b);
			};

		Node* nodeCurrent = nodeStart;
		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);


		list<Node*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd) 
		{
			listNotTestedNodes.sort([](const Node* lhs, const Node* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
			{
				listNotTestedNodes.pop_front();
			}
			if (listNotTestedNodes.empty())
			{
				break;
			}

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true;

			for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
			{
				if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
				{
					listNotTestedNodes.push_back(nodeNeighbour);
				}
				float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

				if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}
			}
		}
	}



	virtual bool OnUserUpdate(float fElaspedTime)
	{
		int nNodeSize = 9;
		int nNodeBorder = 2;

		int nSelectedNodeX = m_mousePosX / nNodeSize;
		int nSelectedNodeY = m_mousePosY / nNodeSize;

		if (m_mouse[0].bReleased)
		{
			if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)
			{
				if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)
				{
					if (m_keys[VK_SHIFT].bHeld)
					{
						nodeStart = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];
					}
					else if (m_keys[VK_CONTROL].bHeld)
					{
						nodeEnd = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];
					}
					else
					{
						nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle = !nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle;
					}

					Solve_AStar();
				}
			}
		}



		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				for (auto n : nodes[y * nMapWidth + x].vecNeighbours)
				{
					DrawLine(x * nNodeSize + nNodeSize / 2, y * nNodeSize + nNodeSize / 2, n->_x * nNodeSize + nNodeSize / 2, n->_y * nNodeSize + nNodeSize / 2, PIXEL_HALF, FG_DARK_BLUE);
				}
			}
		}



		//draw Node
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				// if obstacle -> color is white. 
				Fill(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, (x + 1) * nNodeSize - nNodeBorder, (y + 1) * nNodeSize - nNodeBorder, PIXEL_SOLID, nodes[y * nMapWidth + x].bObstacle ? FG_WHITE : FG_BLUE);

				if (nodes[y * nMapWidth + x].bVisited)
				{
					Fill(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, (x + 1) * nNodeSize - nNodeBorder, (y + 1) * nNodeSize - nNodeBorder, PIXEL_SOLID, FG_BLUE);
				}

				if (&nodes[y * nMapWidth + x] == nodeStart)
				{
					Fill(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, (x + 1) * nNodeSize - nNodeBorder, (y + 1) * nNodeSize - nNodeBorder, PIXEL_SOLID, FG_GREEN);
				}

				if (&nodes[y * nMapWidth + x] == nodeEnd)
				{
					Fill(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, (x + 1) * nNodeSize - nNodeBorder, (y + 1) * nNodeSize - nNodeBorder, PIXEL_SOLID, FG_RED);
				}

			}
		}

		if (nodeEnd != nullptr)
		{
			Node* p = nodeEnd;
			while (p->parent != nullptr)
			{
				DrawLine(p->_x * nNodeSize + nNodeSize / 2, p->_y * nNodeSize + nNodeSize / 2, p->parent->_x * nNodeSize + nNodeSize / 2, p->parent->_y * nNodeSize + nNodeSize / 2, PIXEL_SOLID, FG_YELLOW);
				p = p->parent;
			}
		}

		return true;
	}
};


int main()
{
	PathFinding game;
	game.ConstructConsole(160, 160, 6, 6);
	game.Start();
}
