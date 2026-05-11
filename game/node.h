#ifndef NODE_H
#define NODE_H

#include <cmath>

struct Node {
    int x, y;
    double gCost;   // cost from start to this node
    double hCost;   // heuristic cost from this node to end
    double fCost;   // gCost + hCost
    Node* parent;

    Node() : x(0), y(0), gCost(0.0), hCost(0.0), fCost(0.0), parent(nullptr) {}
    Node(int x_, int y_) : x(x_), y(y_), gCost(0.0), hCost(0.0), fCost(0.0), parent(nullptr) {}

    void calcF() { fCost = gCost + hCost; }

    static double heuristic(int x1, int y1, int x2, int y2) {
        // Manhattan distance
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    static double heuristicDiagonal(int x1, int y1, int x2, int y2) {
        int dx = std::abs(x1 - x2);
        int dy = std::abs(y1 - y2);
        // Octile distance: min(dx,dy)*sqrt(2) + |dx-dy|
        return (dx < dy) ? (dx * 1.41421356237 + (dy - dx))
                         : (dy * 1.41421356237 + (dx - dy));
    }
};

#endif // NODE_H
