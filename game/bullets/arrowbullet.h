#ifndef ARROWBULLET_H
#define ARROWBULLET_H

#include "bullet.h"

class ArrowBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    ArrowBullet(const QPointF& start, const QPointF& target, double damage,
                double slowFactor, double slowDuration,
                double poisonDps, double poisonDuration, const QColor& color);

private:
    double m_slowFactor;
    double m_slowDuration;
    double m_poisonDps;
    double m_poisonDuration;
};

#endif