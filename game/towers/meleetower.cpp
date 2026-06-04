#include "meleetower.h"
#include "../enemies/enemy.h"
#include <algorithm>

MeleeTower::MeleeTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : Tower(type, gridX, gridY, cellSize, offsetX, offsetY)
    , m_cooldown(0), m_attackRadius(2.0)
{
}

void MeleeTower::update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    Q_UNUSED(enemies);
    if (m_cooldown > 0) { m_cooldown -= dt; return; }

    m_pendingEffect.fired = true;
    m_pendingEffect.center = QPointF(m_gridX + 0.5, m_gridY + 0.5);
    m_pendingEffect.radius = m_attackRadius;
    m_pendingEffect.damage = stats().damage;
    m_pendingEffect.slowFactor = stats().slowFactor;
    m_pendingEffect.slowDuration = stats().slowDuration;
    m_pendingEffect.poisonDps = stats().poisonDps;
    m_pendingEffect.poisonDuration = stats().poisonDuration;
    m_pendingEffect.color = stats().color;
    m_cooldown = stats().attackSpeed;
}