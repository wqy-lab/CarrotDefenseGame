#ifndef STATUE_OBSTACLE_H
#define STATUE_OBSTACLE_H

#include "obstacle.h"
#include <QPointF>

class StatueObstacle : public Obstacle {
public:
    StatueObstacle(int gridX, int gridY, const QPointF& pos);

protected:
    void drawBody(QPainter* p, const QPointF& center, int radius) const override;
};

#endif