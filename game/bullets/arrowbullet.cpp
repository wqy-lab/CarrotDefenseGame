#include "arrowbullet.h"
#include "../enemies/enemy.h"
#include "../markers/slowmarker.h"
#include "../markers/poisonmarker.h"

ArrowBullet::ArrowBullet(const QPointF& start, const QPointF& target, double damage,
                         double slowFactor, double slowDuration,
                         double poisonDps, double poisonDuration, const QColor& color)
    : Bullet(start, target, damage, color)
{
    if (slowFactor < 1.0 && slowDuration > 0) {
        m_markers.push_back(std::make_unique<SlowMarker>(slowFactor, slowDuration));
    }
    if (poisonDps > 0 && poisonDuration > 0) {
        m_markers.push_back(std::make_unique<PoisonMarker>(poisonDps, poisonDuration));
    }
}

void ArrowBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);
    for (auto& marker : m_markers) {
        enemy->addMarker(marker->clone());
    }
}