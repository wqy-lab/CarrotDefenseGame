#include "icebullet.h"
#include "../enemies/enemy.h"

IceBullet::IceBullet(const QPointF& start, const QPointF& target, double damage,
                     double slowFactor, double slowDuration, const QColor& color)
    : Bullet(start, target, damage, color)
    , m_slowFactor(slowFactor)
    , m_slowDuration(slowDuration)
{
}

void IceBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    if (m_slowFactor < 1.0 && m_slowDuration > 0) {
        enemy->applySlow(m_slowFactor, m_slowDuration);
    }
}