#include "remotetower.h"
#include "../enemies/enemy.h"
#include "../obstacles/obstacle.h"

RemoteTower::RemoteTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : Tower(type, gridX, gridY, cellSize, offsetX, offsetY)
    , m_cooldown(0)
{
}

void RemoteTower::update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (m_cooldown > 0) { m_cooldown -= dt; return; }

    // Check priority obstacle first
    Obstacle* obsTarget = findObstacleTarget();
    if (obsTarget) {
        m_pendingAttack.fired = true;
        m_pendingAttack.targetPos = QPointF(obsTarget->gridX() + obsTarget->gridWidth() / 2.0,
                                             obsTarget->gridY() + obsTarget->gridHeight() / 2.0);
        m_pendingAttack.damage = stats().damage;
        m_pendingAttack.maxDistance = stats().range;
        m_pendingAttack.splashRadius = stats().splashRadius;
        m_pendingAttack.slowFactor = stats().slowFactor;
        m_pendingAttack.slowDuration = stats().slowDuration;
        m_pendingAttack.poisonDps = stats().poisonDps;
        m_pendingAttack.poisonDuration = stats().poisonDuration;
        m_pendingAttack.chainCount = stats().chainCount;
        m_pendingAttack.color = stats().color;
        m_cooldown = stats().attackSpeed;
        return;
    }

    Enemy* target = findTarget(enemies);
    if (!target) {
        return;
    }

    m_pendingAttack.fired = true;
    m_pendingAttack.targetPos = target->gridPos();
    m_pendingAttack.damage = stats().damage;
    m_pendingAttack.maxDistance = stats().range;
    m_pendingAttack.splashRadius = stats().splashRadius;
    m_pendingAttack.slowFactor = stats().slowFactor;
    m_pendingAttack.slowDuration = stats().slowDuration;
    m_pendingAttack.poisonDps = stats().poisonDps;
    m_pendingAttack.poisonDuration = stats().poisonDuration;
    m_pendingAttack.chainCount = stats().chainCount;
    m_pendingAttack.color = stats().color;
    m_cooldown = stats().attackSpeed;
}

Enemy* RemoteTower::findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) const
{
    if (m_priorityEnemy && m_priorityEnemy->isActive()) {
        double r2 = m_stats.range * m_stats.range;
        double d2 = distTo(*m_priorityEnemy);
        if (d2 <= r2) {
            return m_priorityEnemy;
        }
    }

    Enemy* best = nullptr;
    int bestIndex = -1;
    double r2 = m_stats.range * m_stats.range;
    for (auto& e : enemies) {
        if (!e->isActive()) continue;
        if (e.get() == m_priorityEnemy) continue;
        double d2 = distTo(*e);
        if (d2 <= r2) {
            if (e->pathIndex() > bestIndex) {
                bestIndex = e->pathIndex();
                best = e.get();
            }
        }
    }
    return best;
}

double RemoteTower::distTo(const Enemy& e) const {
    QPointF gp = e.gridPos();
    double dx = gp.x() - (m_gridX + 0.5);
    double dy = gp.y() - (m_gridY + 0.5);
    return dx*dx + dy*dy;
}

double RemoteTower::distTo(const Obstacle& o) const {
    double dx = (o.gridX() + o.gridWidth() / 2.0) - (m_gridX + 0.5);
    double dy = (o.gridY() + o.gridHeight() / 2.0) - (m_gridY + 0.5);
    return dx*dx + dy*dy;
}

Obstacle* RemoteTower::findObstacleTarget() const
{
    if (m_priorityObstacle && m_priorityObstacle->isActive()) {
        double r2 = m_stats.range * m_stats.range;
        double d2 = distTo(*m_priorityObstacle);
        if (d2 <= r2) {
            return m_priorityObstacle;
        }
    }
    return nullptr;
}