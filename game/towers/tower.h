#ifndef TOWER_H
#define TOWER_H

#include <QPointF>
#include <QPainter>
#include <vector>
#include <memory>

class Enemy;

enum class TowerType {
    Arrow,
    Cannon,
    Ice,
    Poison,
    Lightning,
    Sun
};

struct TowerStats {
    int cost;
    double damage;
    double range;
    double attackSpeed;
    double splashRadius;
    double slowFactor;
    double slowDuration;
    double poisonDps;
    double poisonDuration;
    int chainCount;
    QColor color;
};

class Tower {
public:
    virtual ~Tower() = default;

    virtual void update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies) = 0;
    virtual void draw(QPainter& p) const;

    int gridX() const { return m_gridX; }
    int gridY() const { return m_gridY; }
    QPointF centerPos() const { return m_center; }
    double rangePx() const { return m_stats.range * m_cellSize; }
    double cellSize() const { return m_cellSize; }
    int cost() const { return m_stats.cost; }
    TowerStats stats() const { return getStats(); }
    TowerType type() const { return m_type; }

protected:
    Tower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    virtual TowerStats getStats() const = 0;
    virtual void drawBody(QPainter& p, const QPointF& center, double radius) const = 0;

    TowerType m_type;
    TowerStats m_stats;
    int m_gridX, m_gridY;
    QPointF m_center;
    double m_cellSize;
};

#endif