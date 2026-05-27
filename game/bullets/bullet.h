#ifndef BULLET_H
#define BULLET_H

#include <QPointF>
#include <QPainter>
#include <vector>
#include <memory>
#include "../cellentities.h"

class Enemy;

class Bullet {
public:
    virtual ~Bullet() = default;

    void update(double dt, CellEntities& cell);
    void draw(QPainter& p) const;
    bool isActive() const { return m_active; }
    bool hasHit() const { return m_hit; }

    QPointF pos() const { return m_pos; }
    QPointF targetPos() const { return m_targetPos; }
    void setTargetPos(const QPointF& pos) { m_targetPos = pos; }
    void setMaxDistance(double dist) { m_maxDistance = dist; }
    double damage() const { return m_damage; }
    double splashRadius() const { return m_splashRadius; }
    double slowFactor() const { return m_slowFactor; }
    double slowDuration() const { return m_slowDuration; }
    double poisonDps() const { return m_poisonDps; }
    double poisonDuration() const { return m_poisonDuration; }
    int chainCount() const { return m_chainCount; }
    QColor color() const { return m_color; }

protected:
    Bullet(const QPointF& start, const QPointF& target, double damage,
           double splashRadius, double slowFactor, double slowDuration,
           double poisonDps, double poisonDuration, int chainCount, const QColor& color);

    virtual void onHit(Enemy* enemy) = 0;
    virtual void onObstacleHit(class Obstacle* obstacle);

    bool m_hit;
    QPointF m_pos;
    QPointF m_targetPos;
    QPointF m_startPos;
    double m_maxDistance;
    double m_traveledDistance;
    double m_speed;
    double m_damage;
    double m_splashRadius;
    double m_slowFactor;
    double m_slowDuration;
    double m_poisonDps;
    double m_poisonDuration;
    int m_chainCount;
    QColor m_color;
    bool m_active;
};

#endif