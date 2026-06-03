#ifndef POISONTOWER_H
#define POISONTOWER_H

#include "remotetower.h"

class PoisonTower : public RemoteTower {
protected:
    void drawBody(QPainter& p, const QPointF& center, double r) const override;

public:
    PoisonTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY);
};

#endif