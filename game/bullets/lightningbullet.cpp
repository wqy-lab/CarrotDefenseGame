#include "lightningbullet.h"
#include "../enemies/enemy.h"

LightningBullet::LightningBullet(const QPointF& start, const QPointF& direction, double damage,
                                  int chainCount, const QColor& color)
    : Bullet(start, direction, damage, color)
    , m_chainCount(chainCount)
{
}

void LightningBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);

    m_chainedEnemies.insert(enemy);
    QPointF origin = enemy->gridPos();

    while (m_chainCount > 0) {
        Enemy* nextTarget = nullptr;
        double minDist = 3.0;
        minDist = minDist * minDist;

        for (auto& e : enemies) {
            if (!e->isActive()) continue;
            if (m_chainedEnemies.count(e.get()) > 0) continue;
            QPointF d = e->gridPos() - origin;
            double dist2 = d.x()*d.x() + d.y()*d.y();
            if (dist2 < minDist) {
                minDist = dist2;
                nextTarget = e.get();
            }
        }
        if (!nextTarget) break;
        m_chainedEnemies.insert(nextTarget);
        nextTarget->takeDamage(m_damage);
        origin = nextTarget->gridPos();
        --m_chainCount;
    }
}

void LightningBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    Q_UNUSED(p);
    Q_UNUSED(cellSize);
    Q_UNUSED(offsetX);
    Q_UNUSED(offsetY);
}