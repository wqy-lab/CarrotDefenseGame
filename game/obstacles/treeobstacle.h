#ifndef TREE_OBSTACLE_H
#define TREE_OBSTACLE_H

#include "obstacle.h"
#include <QPointF>

class TreeObstacle : public Obstacle {
public:
    TreeObstacle(int gridX, int gridY, int gridW, int gridH, const QPointF& pos);

protected:
    void drawBody(QPainter* p, const QPointF& center, int radius) const override;
};

#endif