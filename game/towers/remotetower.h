#ifndef REMOTETOWER_H
#define REMOTETOWER_H

#include "tower.h"
#include <memory>

class Enemy;

class RemoteTower : public Tower {
public:
    void update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies) override;

    struct AttackResult {
        bool fired = false;
        QPointF targetPos;
        double damage;
        double maxDistance;
        double splashRadius;
        QColor color;
        std::vector<std::unique_ptr<Marker>> markers;
    };

    AttackResult getAttack() { AttackResult r; r.fired = m_pendingAttack.fired; r.targetPos = m_pendingAttack.targetPos; r.damage = m_pendingAttack.damage; r.maxDistance = m_pendingAttack.maxDistance; r.splashRadius = m_pendingAttack.splashRadius; r.color = m_pendingAttack.color; r.markers = std::move(m_pendingAttack.markers); m_pendingAttack = AttackResult(); return r; }
    bool hasPendingAttack() const { return m_pendingAttack.fired; }

    Obstacle* findObstacleTarget() const;

protected:
    RemoteTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    Enemy* findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) const;
    double distTo(const Enemy& e) const;
    double distTo(const Obstacle& o) const;
    std::vector<std::unique_ptr<Marker>> cloneMarkers() const;

    double m_cooldown;
    AttackResult m_pendingAttack;
};

#endif