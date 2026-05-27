#ifndef OBSTACLE_FACTORY_H
#define OBSTACLE_FACTORY_H

#include <memory>
#include <QPointF>

class Obstacle;

enum class ObstacleType {
    Tree,
    Statue
};

std::unique_ptr<Obstacle> createObstacle(ObstacleType type, int gridX, int gridY, const QPointF& pos);

#endif