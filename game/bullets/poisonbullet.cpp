#include "poisonbullet.h"
#include "../enemies/enemy.h"
#include "../markers/poisonmarker.h"

PoisonBullet::PoisonBullet(const QPointF& start, const QPointF& target, double damage,
                            double poisonDps, double poisonDuration, const QColor& color)
    : Bullet(start, target, damage, color)
{
    if (poisonDps > 0 && poisonDuration > 0) {
        m_marker = std::make_unique<PoisonMarker>(poisonDps, poisonDuration);
    }
}

void PoisonBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    if (m_marker) {
        enemy->addMarker(m_marker->clone());
    }
}