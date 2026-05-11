#ifndef ARROWTOWER_H
#define ARROWTOWER_H

#include "remotetower.h"

class ArrowTower : public RemoteTower {
protected:
    TowerStats getStats() const override;
    void drawBody(QPainter& p, const QPointF& center, double r) const override;

public:
    ArrowTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY);
};

#endif