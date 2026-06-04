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

    void update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell);
    virtual void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const;
    bool isActive() const { return m_active; }
    bool hasHit() const { return m_hit; }

    QPointF pos() const { return m_pos; }
    void setMaxDistance(double dist) { m_maxDistance = dist; }
    void setDirection(const QPointF& dir) { m_direction = dir; }
    void setGridBounds(int cols, int rows) { m_gridCols = cols; m_gridRows = rows; }
    double damage() const { return m_damage; }
    QColor color() const { return m_color; }

protected:
    Bullet(const QPointF& start, const QPointF& target, double damage, const QColor& color);

    virtual void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) = 0;
    virtual void onObstacleHit(class Obstacle* obstacle);

protected:
    bool m_hit;
    QPointF m_pos;
    QPointF m_direction;
    QPointF m_startPos;
    double m_maxDistance;
    double m_traveledDistance;
    double m_speed;
    double m_damage;
    QColor m_color;
    bool m_active;
    int m_gridCols = 15;
    int m_gridRows = 12;
};

#endif