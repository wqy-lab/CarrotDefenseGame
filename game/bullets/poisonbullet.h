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
    PoisonBullet(const QPointF& start, const QPointF& target, double damage,
                 double poisonDps, double poisonDuration, const QColor& color);

private:
    std::unique_ptr<Marker> m_marker;
};

#endif