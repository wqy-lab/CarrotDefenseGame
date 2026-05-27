#include "obstaclefactory.h"
#include "treeobstacle.h"
#include "statueobstacle.h"

std::unique_ptr<Obstacle> createObstacle(ObstacleType type, int gridX, int gridY, const QPointF& pos) {
    switch (type) {
    case ObstacleType::Tree:
        return std::make_unique<TreeObstacle>(gridX, gridY, pos);
    case ObstacleType::Statue:
        return std::make_unique<StatueObstacle>(gridX, gridY, pos);
    }
    return std::make_unique<TreeObstacle>(gridX, gridY, pos);
}