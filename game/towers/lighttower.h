#ifndef LIGHTTOWER_H
#define LIGHTTOWER_H

#include "remotetower.h"

class LightningTower : public RemoteTower {
protected:
    void drawBody(QPainter& p, const QPointF& center, double r) const override;

public:
    LightningTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY);
};

#endif