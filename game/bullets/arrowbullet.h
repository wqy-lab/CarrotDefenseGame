#ifndef ARROWBULLET_H
#define ARROWBULLET_H

#include "bullet.h"
#include <vector>
#include <memory>
#include "../markers/marker.h"

class ArrowBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    ArrowBullet(const QPointF& start, const QPointF& target, double damage,
                double slowFactor, double slowDuration,
                double poisonDps, double poisonDuration, const QColor& color);

private:
    std::vector<std::unique_ptr<Marker>> m_markers;
};

#endif