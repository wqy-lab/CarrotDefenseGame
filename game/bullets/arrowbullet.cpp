#include "arrowbullet.h"
#include "../enemies/enemy.h"

ArrowBullet::ArrowBullet(const QPointF& start, const QPointF& target, double damage,
                         double slowFactor, double slowDuration,
                         double poisonDps, double poisonDuration, const QColor& color)
    : Bullet(start, target, damage, color)
    , m_slowFactor(slowFactor)
    , m_slowDuration(slowDuration)
    , m_poisonDps(poisonDps)
    , m_poisonDuration(poisonDuration)
{
}

void ArrowBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);

    if (m_slowFactor < 1.0 && m_slowDuration > 0) {
        enemy->applySlow(m_slowFactor, m_slowDuration);
    }

    if (m_poisonDps > 0 && m_poisonDuration > 0) {
        enemy->applyPoison(m_poisonDps, m_poisonDuration);
    }
}