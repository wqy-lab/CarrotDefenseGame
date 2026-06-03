#ifndef NORMALENEMY_H
#define NORMALENEMY_H

#include <vector>
#include <QPointF>
#include "enemy.h"

class NormalEnemy : public Enemy {
public:
    NormalEnemy(const std::vector<QPointF>& path);

protected:
    void drawBody(QPainter& p, const QPointF& center, int r) const override;
};

#endif
