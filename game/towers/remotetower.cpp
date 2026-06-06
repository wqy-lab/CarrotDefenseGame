#include "remotetower.h"
#include "../enemies/enemy.h"

RemoteTower::RemoteTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : Tower(type, gridX, gridY, cellSize, offsetX, offsetY)
    , m_cooldown(0)
{
}

void RemoteTower::update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (m_wavesRemaining > 0) {
        m_waveTimer -= dt;
        if (m_waveTimer <= 0) {
            m_waveAttack.fired = true;
            m_wavesRemaining--;
            m_waveTimer = m_waveAttack.waveDelay;
        } else {
            return;
        }
    }

    if (m_cooldown > 0) { m_cooldown -= dt; return; }

    Obstacle* obsTarget = findObstacleTarget();
    if (obsTarget) {
        m_pendingAttack.fired = true;
        m_pendingAttack.targetPos = QPointF(obsTarget->gridX() + obsTarget->gridWidth() / 2.0,
                                             obsTarget->gridY() + obsTarget->gridHeight() / 2.0);
        m_pendingAttack.damage = damage();
        m_pendingAttack.maxDistance = range();
        m_pendingAttack.splashRadius = 0;
        m_pendingAttack.color = color();
        m_pendingAttack.penetration = penetration();
        m_pendingAttack.shotCount = shotCount();
        m_pendingAttack.spreadAngle = spreadAngle();
        m_pendingAttack.waveCount = waveCount();
        m_pendingAttack.waveDelay = waveDelay();
        m_pendingAttack.markers = cloneMarkers();
        if (waveCount() > 1) {
            m_waveAttack.targetPos = m_pendingAttack.targetPos;
            m_waveAttack.damage = m_pendingAttack.damage;
            m_waveAttack.maxDistance = m_pendingAttack.maxDistance;
            m_waveAttack.splashRadius = m_pendingAttack.splashRadius;
            m_waveAttack.color = m_pendingAttack.color;
            m_waveAttack.penetration = m_pendingAttack.penetration;
            m_waveAttack.shotCount = m_pendingAttack.shotCount;
            m_waveAttack.spreadAngle = m_pendingAttack.spreadAngle;
            m_waveAttack.waveCount = m_pendingAttack.waveCount;
            m_waveAttack.waveDelay = m_pendingAttack.waveDelay;
            m_waveAttack.fired = false;
            m_waveAttack.markers = cloneMarkers();
            m_wavesRemaining = waveCount() - 1;
            m_waveTimer = waveDelay();
        }
        m_cooldown = attackSpeed();
        return;
    }

    Enemy* target = findTarget(enemies);
    if (!target) return;

    m_pendingAttack.fired = true;
    m_pendingAttack.targetPos = target->gridPos();
    m_pendingAttack.damage = damage();
    m_pendingAttack.maxDistance = range();
    m_pendingAttack.splashRadius = 0;
    m_pendingAttack.color = color();
    m_pendingAttack.penetration = penetration();
    m_pendingAttack.shotCount = shotCount();
    m_pendingAttack.spreadAngle = spreadAngle();
    m_pendingAttack.waveCount = waveCount();
    m_pendingAttack.waveDelay = waveDelay();
    m_pendingAttack.markers = cloneMarkers();
    if (waveCount() > 1) {
        m_waveAttack.targetPos = m_pendingAttack.targetPos;
        m_waveAttack.damage = m_pendingAttack.damage;
        m_waveAttack.maxDistance = m_pendingAttack.maxDistance;
        m_waveAttack.splashRadius = m_pendingAttack.splashRadius;
        m_waveAttack.color = m_pendingAttack.color;
        m_waveAttack.penetration = m_pendingAttack.penetration;
        m_waveAttack.shotCount = m_pendingAttack.shotCount;
        m_waveAttack.spreadAngle = m_pendingAttack.spreadAngle;
        m_waveAttack.waveCount = m_pendingAttack.waveCount;
        m_waveAttack.waveDelay = m_pendingAttack.waveDelay;
        m_waveAttack.fired = false;
        m_waveAttack.markers = cloneMarkers();
        m_wavesRemaining = waveCount() - 1;
        m_waveTimer = waveDelay();
    }
    m_cooldown = attackSpeed();
}

Enemy* RemoteTower::findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) const
{
    if (m_priorityEnemy && m_priorityEnemy->isActive()) {
        double r2 = range() * range();
        double d2 = distTo(*m_priorityEnemy);
        if (d2 <= r2) {
            return m_priorityEnemy;
        }
    }

    Enemy* best = nullptr;
    int bestIndex = -1;
    double r2 = range() * range();
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