#ifndef MELEETOWER_H
#define MELEETOWER_H

#include "tower.h"
#include "../markers/marker.h"
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
        QColor color;
        std::vector<std::unique_ptr<Marker>> markers;
    };

    AreaEffect getEffect() { AreaEffect e; e.fired = m_pendingEffect.fired; e.center = m_pendingEffect.center; e.radius = m_pendingEffect.radius; e.damage = m_pendingEffect.damage; e.color = m_pendingEffect.color; e.markers = std::move(m_pendingEffect.markers); m_pendingEffect = AreaEffect(); return e; }
    bool hasPendingEffect() const { return m_pendingEffect.fired; }

protected:
    MeleeTower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    std::vector<std::unique_ptr<Marker>> cloneMarkers() const;

    double m_cooldown;
    double m_attackRadius;
    AreaEffect m_pendingEffect;
};

#endif