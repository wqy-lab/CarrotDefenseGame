#ifndef BOSSENEMY_H
#define BOSSENEMY_H

#include <vector>
#include <QPointF>
#include "enemy.h"

class BossEnemy : public Enemy {
public:
    BossEnemy(const std::vector<QPointF>& path);

protected:
    void drawBody(QPainter& p, const QPointF& center, int r) const override;
};

#endif
