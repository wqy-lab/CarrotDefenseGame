#include "remotetower.h"
#include "../enemies/enemy.h"

RemoteTower::RemoteTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : Tower(type, gridX, gridY, cellSize, offsetX, offsetY)
    , m_cooldown(0)
{
}

void RemoteTower::update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (m_cooldown > 0) { m_cooldown -= dt; return; }

    Enemy* target = findTarget(enemies);
    if (!target) return;

    m_pendingAttack.fired = true;
    m_pendingAttack.targetPos = target->pos();
    m_pendingAttack.damage = getStats().damage;
    m_pendingAttack.maxDistance = rangePx();
    m_pendingAttack.splashRadius = getStats().splashRadius * m_cellSize;
    m_pendingAttack.slowFactor = getStats().slowFactor;
    m_pendingAttack.slowDuration = getStats().slowDuration;
    m_pendingAttack.poisonDps = getStats().poisonDps;
    m_pendingAttack.poisonDuration = getStats().poisonDuration;
    m_pendingAttack.chainCount = getStats().chainCount;
    m_pendingAttack.color = getStats().color;
    m_cooldown = getStats().attackSpeed;
}

Enemy* RemoteTower::findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) const
{
    Enemy* best = nullptr;
    int bestIndex = -1;
    double r2 = rangePx() * rangePx();
    for (auto& e : enemies) {
        if (!e->isActive()) continue;
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
    QPointF d = e.pos() - m_center;
    return d.x()*d.x() + d.y()*d.y();
}