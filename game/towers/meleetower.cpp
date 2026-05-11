#include "meleetower.h"
#include "../enemies/enemy.h"
#include <algorithm>

MeleeTower::MeleeTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : Tower(type, gridX, gridY, cellSize, offsetX, offsetY)
    , m_cooldown(0), m_attackRadius(2.0 * cellSize)
{
}

void MeleeTower::update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (m_cooldown > 0) { m_cooldown -= dt; return; }

    m_pendingEffect.fired = true;
    m_pendingEffect.center = centerPos();
    m_pendingEffect.radius = m_attackRadius;
    m_pendingEffect.damage = getStats().damage;
    m_pendingEffect.slowFactor = getStats().slowFactor;
    m_pendingEffect.slowDuration = getStats().slowDuration;
    m_pendingEffect.poisonDps = getStats().poisonDps;
    m_pendingEffect.poisonDuration = getStats().poisonDuration;
    m_pendingEffect.color = getStats().color;
    m_cooldown = getStats().attackSpeed;
}