#ifndef POISONBULLET_H
#define POISONBULLET_H

#include "bullet.h"

class PoisonBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    PoisonBullet(const QPointF& start, const QPointF& target, double damage,
                 double poisonDps, double poisonDuration, const QColor& color);

private:
    double m_poisonDps;
    double m_poisonDuration;
};

#endif