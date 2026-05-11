#ifndef CANNTOWER_H
#define CANNTOWER_H

#include "remotetower.h"

class CannonTower : public RemoteTower {
protected:
    TowerStats getStats() const override;
    void drawBody(QPainter& p, const QPointF& center, double r) const override;

public:
    CannonTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY);
};

#endif