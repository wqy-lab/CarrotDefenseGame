#ifndef BULLET_H
#define BULLET_H

#include <QPointF>
#include <QPainter>
#include <vector>
#include <memory>
#include <set>
#include "../cellentities.h"

class Enemy;

class Bullet {
public:
    virtual ~Bullet() = default;

    void update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell);
    void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const;
    bool isActive() const { return m_active; }
    bool hasHit() const { return m_hit; }

    QPointF pos() const { return m_pos; }
    void setMaxDistance(double dist) { m_maxDistance = dist; }
    void setDirection(const QPointF& dir) { m_direction = dir; }
    void setGridBounds(int cols, int rows) { m_gridCols = cols; m_gridRows = rows; }
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

    virtual void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) = 0;
    virtual void onObstacleHit(class Obstacle* obstacle);

    bool m_hit;
    QPointF m_pos;
    QPointF m_direction;  // Normalized direction vector for movement
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
    int m_gridCols = 15;
    int m_gridRows = 12;
    double m_cellSize;
    double m_offsetX;
    double m_offsetY;
    std::set<Enemy*> m_chainedEnemies;
};

#endif