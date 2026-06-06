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
        int penetration = 0;
        int shotCount = 1;
        int spreadAngle = 0;
        int waveCount = 1;
        double waveDelay = 0;
        std::vector<std::unique_ptr<Marker>> markers;
    };

    AttackResult getAttack() {
        if (m_pendingAttack.fired) {
            AttackResult r;
            r.fired = true;
            r.targetPos = m_pendingAttack.targetPos;
            r.damage = m_pendingAttack.damage;
            r.maxDistance = m_pendingAttack.maxDistance;
            r.splashRadius = m_pendingAttack.splashRadius;
            r.color = m_pendingAttack.color;
            r.penetration = m_pendingAttack.penetration;
            r.shotCount = m_pendingAttack.shotCount;
            r.spreadAngle = m_pendingAttack.spreadAngle;
            r.waveCount = m_pendingAttack.waveCount;
            r.waveDelay = m_pendingAttack.waveDelay;
            r.markers = std::move(m_pendingAttack.markers);
            m_pendingAttack = AttackResult();
            return r;
        }
        if (m_waveAttack.fired) {
            AttackResult r;
            r.fired = true;
            r.targetPos = m_waveAttack.targetPos;
            r.damage = m_waveAttack.damage;
            r.maxDistance = m_waveAttack.maxDistance;
            r.splashRadius = m_waveAttack.splashRadius;
            r.color = m_waveAttack.color;
            r.penetration = m_waveAttack.penetration;
            r.shotCount = m_waveAttack.shotCount;
            r.spreadAngle = m_waveAttack.spreadAngle;
            r.waveCount = m_waveAttack.waveCount;
            r.waveDelay = m_waveAttack.waveDelay;
            for (auto& m : m_waveAttack.markers) {
                r.markers.push_back(m->clone());
            }
            m_waveAttack.fired = false;
            return r;
        }
        return AttackResult();
    }
    bool hasPendingAttack() const { return m_pendingAttack.fired || m_waveAttack.fired; }

protected:
    RemoteTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    Enemy* findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) const;
    double distTo(const Enemy& e) const;

    double m_cooldown;
    AttackResult m_pendingAttack;
    AttackResult m_waveAttack;
    int m_wavesRemaining = 0;
    double m_waveTimer = 0;
};

#endif