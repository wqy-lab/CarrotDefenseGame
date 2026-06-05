#ifndef CANNONBULLET_H
#define CANNONBULLET_H

#include "bullet.h"

class CannonBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    CannonBullet(const QPointF& start, const QPointF& target, double damage,
                 double splashRadius, const QColor& color);

    void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const override;

private:
    double m_splashRadius;
};

#endif