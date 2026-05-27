#include "normalbullet.h"
#include "../enemies/enemy.h"
#include <QtMath>

NormalBullet::NormalBullet(const QPointF& start, const QPointF& target, double damage,
                           double splashRadius, double slowFactor, double slowDuration,
                           double poisonDps, double poisonDuration, int chainCount, const QColor& color)
    : Bullet(start, target, damage, splashRadius, slowFactor, slowDuration,
             poisonDps, poisonDuration, chainCount, color)
{
}

void NormalBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    enemy->takeDamage(m_damage);

    if (m_slowFactor < 1.0 && m_slowDuration > 0) {
        enemy->applySlow(m_slowFactor, m_slowDuration);
    }

    if (m_poisonDps > 0 && m_poisonDuration > 0) {
        enemy->applyPoison(m_poisonDps, m_poisonDuration);
    }

    // Splash damage
    if (m_splashRadius > 0) {
        for (auto& e : enemies) {
            if (!e->isActive() || e.get() == enemy) continue;
            QPointF d = e->pos() - m_pos;
            double dist2 = d.x()*d.x() + d.y()*d.y();
            if (dist2 <= m_splashRadius * m_splashRadius) {
                e->takeDamage(m_damage * 0.5);
            }
        }
    }

    // Chain lightning - find nearest unchained enemy in range
    m_chainedEnemies.insert(enemy);  // mark initial hit so we don't chain back to it
    QPointF origin = enemy->pos();
    while (m_chainCount > 0) {
        Enemy* nextTarget = nullptr;
        double minDist = 150.0 * 150.0;
        for (auto& e : enemies) {
            if (!e->isActive()) continue;
            if (m_chainedEnemies.count(e.get()) > 0) continue;
            QPointF d = e->pos() - origin;
            double dist2 = d.x()*d.x() + d.y()*d.y();
            if (dist2 < minDist) {
                minDist = dist2;
                nextTarget = e.get();
            }
        }
        if (!nextTarget) break;
        m_chainedEnemies.insert(nextTarget);
        nextTarget->takeDamage(m_damage);
        origin = nextTarget->pos();
        --m_chainCount;
    }
}
