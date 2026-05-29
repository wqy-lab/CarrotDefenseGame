#include "obstaclefactory.h"
#include "treeobstacle.h"
#include "statueobstacle.h"

std::unique_ptr<Obstacle> createObstacle(ObstacleType type, int gridX, int gridY, int gridW, int gridH, const QPointF& pos) {
    switch (type) {
    case ObstacleType::Tree:
        return std::make_unique<TreeObstacle>(gridX, gridY, gridW, gridH, pos);
    case ObstacleType::Statue:
        return std::make_unique<StatueObstacle>(gridX, gridY, gridW, gridH, pos);
    }
    return std::make_unique<TreeObstacle>(gridX, gridY, gridW, gridH, pos);
}