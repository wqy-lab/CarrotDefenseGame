#include "cannonbullet.h"
#include "../enemies/enemy.h"

CannonBullet::CannonBullet(const QPointF& start, const QPointF& target, double damage,
                            double splashRadius, const QColor& color)
    : Bullet(start, target, damage, color)
    , m_splashRadius(splashRadius)
{
}

void CannonBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);

    if (m_splashRadius > 0) {
        for (auto& e : enemies) {
            if (!e->isActive() || e.get() == enemy) continue;
            QPointF d = e->gridPos() - m_pos;
            double dist2 = d.x()*d.x() + d.y()*d.y();
            if (dist2 <= m_splashRadius * m_splashRadius) {
                e->takeDamage(m_damage * 0.5);
            }
        }
    }
}