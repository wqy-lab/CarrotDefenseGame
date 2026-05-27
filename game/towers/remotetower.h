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
        double slowFactor;
        double slowDuration;
        double poisonDps;
        double poisonDuration;
        int chainCount;
        QColor color;
    };

    AttackResult getAttack() { auto r = m_pendingAttack; m_pendingAttack = AttackResult(); return r; }
    bool hasPendingAttack() const { return m_pendingAttack.fired; }

protected:
    RemoteTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    Enemy* findTarget(const std::vector<std::unique_ptr<Enemy>>& enemies) const;
    double distTo(const Enemy& e) const;

    double m_cooldown;
    AttackResult m_pendingAttack;
};

#endif