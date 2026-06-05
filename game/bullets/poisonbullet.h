#ifndef POISONBULLET_H
#define POISONBULLET_H

#include "bullet.h"
#include <vector>
#include <memory>
#include "../markers/marker.h"

class PoisonBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    PoisonBullet(const QPointF& start, const QPointF& target, double damage, const QColor& color);

    void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const override;
};

#endif