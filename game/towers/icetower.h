#ifndef ICETOWER_H
#define ICETOWER_H

#include "remotetower.h"

class IceTower : public RemoteTower {
protected:
    TowerStats getStats() const override;
    void drawBody(QPainter& p, const QPointF& center, double r) const override;

public:
    IceTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY);
};

#endif