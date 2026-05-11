#ifndef SUNTOWER_H
#define SUNTOWER_H

#include "meleetower.h"

class SunTower : public MeleeTower {
protected:
    TowerStats getStats() const override;
    void drawBody(QPainter& p, const QPointF& center, double r) const override;

public:
    SunTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY);
};

#endif