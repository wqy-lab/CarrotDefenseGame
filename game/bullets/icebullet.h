#ifndef ICEBULLET_H
#define ICEBULLET_H

#include "bullet.h"

class IceBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    IceBullet(const QPointF& start, const QPointF& target, double damage,
              double slowFactor, double slowDuration, const QColor& color);

private:
    double m_slowFactor;
    double m_slowDuration;
};

#endif