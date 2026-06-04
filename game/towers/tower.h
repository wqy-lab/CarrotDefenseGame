#ifndef TOWER_H
#define TOWER_H

#include <QPointF>
#include <QPainter>
#include <QString>
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
    virtual void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const;

    int gridX() const { return m_gridX; }
    int gridY() const { return m_gridY; }
    QPointF centerPos(double cellSize, double offsetX, double offsetY) const;
    double rangePx() const { return m_stats.range * m_cellSize; }
    double cellSize() const { return m_cellSize; }
    int cost() const { return m_baseCost; }
    TowerStats stats() const { return m_stats; }
    TowerType type() const { return m_type; }
    int level() const { return m_level; }
    int upgradeCost() const { return m_baseCost * (1 << (m_level - 1)); }
    int sellValue() const { return static_cast<int>(m_totalInvested / 2.0); }
    bool upgrade();
    QString name() const;

protected:
    Tower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    virtual void drawBody(QPainter& p, const QPointF& center, double radius) const = 0;

    TowerType m_type;
    TowerStats m_stats;
    int m_gridX, m_gridY;
    double m_cellSize;
    int m_level = 1;
    int m_baseCost = 0;
    double m_totalInvested = 0;
    TowerStats m_baseStats;

public:
    void setPriorityEnemy(class Enemy* e) { m_priorityEnemy = e; }
    class Enemy* priorityEnemy() const { return m_priorityEnemy; }
    void setPriorityObstacle(class Obstacle* o) { m_priorityObstacle = o; }
    class Obstacle* priorityObstacle() const { return m_priorityObstacle; }

protected:
    class Enemy* m_priorityEnemy = nullptr;
    class Obstacle* m_priorityObstacle = nullptr;
};

#endif