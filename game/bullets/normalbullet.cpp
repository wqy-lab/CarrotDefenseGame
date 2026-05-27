#include "normalbullet.h"
#include "../enemies/enemy.h"

NormalBullet::NormalBullet(const QPointF& start, const QPointF& target, double damage,
                           double splashRadius, double slowFactor, double slowDuration,
                           double poisonDps, double poisonDuration, int chainCount, const QColor& color)
    : Bullet(start, target, damage, splashRadius, slowFactor, slowDuration,
             poisonDps, poisonDuration, chainCount, color)
{
}

void NormalBullet::onHit(Enemy* enemy)
{
    enemy->takeDamage(m_damage);

    if (m_slowFactor < 1.0 && m_slowDuration > 0) {
        enemy->applySlow(m_slowFactor, m_slowDuration);
    }

    if (m_poisonDps > 0 && m_poisonDuration > 0) {
        enemy->applyPoison(m_poisonDps, m_poisonDuration);
    }
}