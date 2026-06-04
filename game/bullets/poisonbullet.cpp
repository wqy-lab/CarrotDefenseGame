#include "poisonbullet.h"
#include "../enemies/enemy.h"

PoisonBullet::PoisonBullet(const QPointF& start, const QPointF& target, double damage,
                            double poisonDps, double poisonDuration, const QColor& color)
    : Bullet(start, target, damage, color)
    , m_poisonDps(poisonDps)
    , m_poisonDuration(poisonDuration)
{
}

void PoisonBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    if (m_poisonDps > 0 && m_poisonDuration > 0) {
        enemy->applyPoison(m_poisonDps, m_poisonDuration);
    }
}