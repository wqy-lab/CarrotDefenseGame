#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "node.h"
#include <vector>
#include <unordered_set>
#include <queue>
#include <functional>
#include <utility>

// Hash for std::pair<int,int> used in closed set
struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

struct NodePtrCompare {
    bool operator()(const Node* a, const Node* b) const {
        return a->fCost > b->fCost; // min-heap: lower fCost = higher priority
    }
};

class Pathfinder {
public:
    Pathfinder();

    void setGrid(int cols, int rows);
    void setWalkable(int x, int y, bool walkable);
    bool isWalkable(int x, int y) const;
    int cols() const { return m_cols; }
    int rows() const { return m_rows; }

    // Enable/disable diagonal movement
    void setDiagonal(bool enable) { m_diagonal = enable; }
    bool diagonal() const { return m_diagonal; }

    // Run A* from (sx,sy) to (ex,ey). Returns true if path found.
    bool findPath(int sx, int sy, int ex, int ey);

    // Results
    const std::vector<std::pair<int, int>>& getPath() const { return m_path; }
    const std::vector<std::pair<int, int>>& getExplored() const { return m_explored; }
    const std::vector<std::pair<int, int>>& getFrontierSnapshot() const { return m_frontierSnapshot; }
    int nodesExplored() const { return static_cast<int>(m_explored.size()); }

private:
    int m_cols, m_rows;
    bool m_diagonal;
    std::vector<std::vector<bool>> m_walkable;

    // A* internal
    std::vector<std::vector<Node>> m_nodes;

    // Results
    std::vector<std::pair<int, int>> m_path;
    std::vector<std::pair<int, int>> m_explored;
    std::vector<std::pair<int, int>> m_frontierSnapshot;

    void reconstructPath(Node* endNode);
    bool isValid(int x, int y) const;
};

#endif // PATHFINDER_H
