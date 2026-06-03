#ifndef NORMALBULLET_H
#define NORMALBULLET_H

#include "bullet.h"
#include <QPixmap>

class NormalBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    NormalBullet(const QPointF& start, const QPointF& target, double damage,
                 double splashRadius, double slowFactor, double slowDuration,
                 double poisonDps, double poisonDuration, int chainCount,
                 const QColor& color, const QPixmap& texture = QPixmap());
};

#endif