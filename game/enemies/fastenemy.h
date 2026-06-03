#ifndef FASTENEMY_H
#define FASTENEMY_H

#include <vector>
#include <QPointF>
#include "enemy.h"

class FastEnemy : public Enemy {
public:
    FastEnemy(const std::vector<QPointF>& path);

protected:
    void drawBody(QPainter& p, const QPointF& center, int r) const override;
};

#endif
