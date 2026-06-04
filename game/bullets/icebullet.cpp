#include "icebullet.h"
#include "../enemies/enemy.h"
#include "../markers/slowmarker.h"

IceBullet::IceBullet(const QPointF& start, const QPointF& target, double damage,
                     double slowFactor, double slowDuration, const QColor& color)
    : Bullet(start, target, damage, color)
{
    if (slowFactor < 1.0 && slowDuration > 0) {
        m_marker = std::make_unique<SlowMarker>(slowFactor, slowDuration);
    }
}

void IceBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    if (m_marker) {
        enemy->addMarker(m_marker->clone());
    }
}