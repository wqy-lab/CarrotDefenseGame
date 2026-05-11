#ifndef MELEETOWER_H
#define MELEETOWER_H

#include "tower.h"
#include <vector>
#include <memory>

class Enemy;

class MeleeTower : public Tower {
public:
    void update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies) override;

    struct AreaEffect {
        bool fired = false;
        QPointF center;
        double radius;
        double damage;
        double slowFactor;
        double slowDuration;
        double poisonDps;
        double poisonDuration;
        QColor color;
    };

    AreaEffect getEffect() { auto e = m_pendingEffect; m_pendingEffect = AreaEffect(); return e; }
    bool hasPendingEffect() const { return m_pendingEffect.fired; }

protected:
    MeleeTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    double m_cooldown;
    double m_attackRadius;
    AreaEffect m_pendingEffect;
};

#endif