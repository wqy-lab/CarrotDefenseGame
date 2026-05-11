#include "pathfinder.h"
#include <algorithm>
#include <cmath>

Pathfinder::Pathfinder()
    : m_cols(0), m_rows(0), m_diagonal(false)
{
}

void Pathfinder::setGrid(int cols, int rows)
{
    m_cols = cols;
    m_rows = rows;
    m_walkable.assign(rows, std::vector<bool>(cols, true));
    m_nodes.assign(rows, std::vector<Node>(cols));
    // Initialize node coordinates
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            m_nodes[y][x] = Node(x, y);
}

void Pathfinder::setWalkable(int x, int y, bool walkable)
{
    if (x >= 0 && x < m_cols && y >= 0 && y < m_rows)
        m_walkable[y][x] = walkable;
}

bool Pathfinder::isWalkable(int x, int y) const
{
    if (x < 0 || x >= m_cols || y < 0 || y >= m_rows)
        return false;
    return m_walkable[y][x];
}

bool Pathfinder::isValid(int x, int y) const
{
    return x >= 0 && x < m_cols && y >= 0 && y < m_rows && m_walkable[y][x];
}

bool Pathfinder::findPath(int sx, int sy, int ex, int ey)
{
    m_path.clear();
    m_explored.clear();
    m_frontierSnapshot.clear();

    if (!isValid(sx, sy) || !isValid(ex, ey))
        return false;

    if (sx == ex && sy == ey) {
        m_path.push_back({sx, sy});
        m_explored.push_back({sx, sy});
        return true;
    }

    // Reset all nodes
    for (int y = 0; y < m_rows; ++y)
        for (int x = 0; x < m_cols; ++x)
            m_nodes[y][x] = Node(x, y);

    // Open set (priority queue) and closed set
    std::priority_queue<Node*, std::vector<Node*>, NodePtrCompare> openSet;
    std::unordered_set<std::pair<int, int>, PairHash> closedSet;
    std::unordered_set<std::pair<int, int>, PairHash> openSetLookup;

    // Directions: 4-directional
    const int dx4[] = { 0, 1, 0, -1 };
    const int dy4[] = { -1, 0, 1, 0 };
    const double cost4 = 1.0;

    // Directions: 8-directional (diagonal)
    const int dx8[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    const int dy8[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    const double cost8[] = { 1.0, 1.41421356237, 1.0, 1.41421356237,
                              1.0, 1.41421356237, 1.0, 1.41421356237 };

    const int* dx = m_diagonal ? dx8 : dx4;
    const int* dy = m_diagonal ? dy8 : dy4;
    const double* costs = m_diagonal ? cost8 : nullptr;
    const int dirCount = m_diagonal ? 8 : 4;

    // Initialize start node
    Node& startNode = m_nodes[sy][sx];
    startNode.gCost = 0.0;
    startNode.hCost = m_diagonal
        ? Node::heuristicDiagonal(sx, sy, ex, ey)
        : Node::heuristic(sx, sy, ex, ey);
    startNode.calcF();
    openSet.push(&startNode);
    openSetLookup.insert({sx, sy});

    int exploredCount = 0;

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        std::pair<int, int> curPos(current->x, current->y);
        openSetLookup.erase(curPos);

        if (closedSet.count(curPos))
            continue;

        closedSet.insert(curPos);
        m_explored.push_back({current->x, current->y});
        ++exploredCount;

        // Reached goal
        if (current->x == ex && current->y == ey) {
            reconstructPath(current);

            // Capture frontier snapshot
            std::priority_queue<Node*, std::vector<Node*>, NodePtrCompare> temp = openSet;
            while (!temp.empty()) {
                Node* n = temp.top(); temp.pop();
                if (!closedSet.count({n->x, n->y}))
                    m_frontierSnapshot.push_back({n->x, n->y});
            }
            return true;
        }

        // Explore neighbors
        for (int i = 0; i < dirCount; ++i) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];
            std::pair<int, int> nPos(nx, ny);

            if (!isValid(nx, ny) || closedSet.count(nPos))
                continue;

            // For diagonal movement, prevent corner-cutting through walls
            if (m_diagonal && i % 2 == 1) {
                // Check the two adjacent orthogonal cells
                int ax = current->x + dx[(i + 1) % 8];
                int ay = current->y + dy[(i + 1) % 8];
                int bx = current->x + dx[(i + 7) % 8];
                int by = current->y + dy[(i + 7) % 8];
                if (!isWalkable(ax, ay) && !isWalkable(bx, by))
                    continue;
            }

            double moveCost = m_diagonal ? costs[i] : cost4;
            double tentativeG = current->gCost + moveCost;

            Node& neighbor = m_nodes[ny][nx];

            if (openSetLookup.count(nPos) && tentativeG >= neighbor.gCost)
                continue;

            neighbor.parent = current;
            neighbor.gCost = tentativeG;
            neighbor.hCost = m_diagonal
                ? Node::heuristicDiagonal(nx, ny, ex, ey)
                : Node::heuristic(nx, ny, ex, ey);
            neighbor.calcF();

            if (!openSetLookup.count(nPos)) {
                openSet.push(&neighbor);
                openSetLookup.insert(nPos);
            }
        }
    }

    // No path found — capture last frontier
    std::priority_queue<Node*, std::vector<Node*>, NodePtrCompare> temp = openSet;
    while (!temp.empty()) {
        Node* n = temp.top(); temp.pop();
        m_frontierSnapshot.push_back({n->x, n->y});
    }
    return false;
}

void Pathfinder::reconstructPath(Node* endNode)
{
    Node* current = endNode;
    while (current != nullptr) {
        m_path.push_back({current->x, current->y});
        current = current->parent;
    }
    std::reverse(m_path.begin(), m_path.end());
}
