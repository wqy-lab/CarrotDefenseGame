#ifndef TOWER_H
#define TOWER_H

#include <QPointF>
#include <QPainter>
#include <QString>
#include <QColor>
#include <vector>
#include <memory>
#include <array>
#include "../markers/marker.h"
#include "../obstacles/obstacle.h"

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
    QColor color;
    int penetration = 0;
    int shotCount = 1;
    int spreadAngle = 0;
    int waveCount = 1;
    double waveDelay = 0;
};

class Tower {
public:
    virtual ~Tower() = default;

    virtual void update(double dt, const std::vector<std::unique_ptr<Enemy>>& enemies) = 0;
    virtual void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const;

    int gridX() const { return m_gridX; }
    int gridY() const { return m_gridY; }
    QPointF centerPos(double cellSize, double offsetX, double offsetY) const;
    double rangePx() const { return range() * m_cellSize; }
    double cellSize() const { return m_cellSize; }
    TowerType type() const { return m_type; }
    int level() const { return m_level; }
    static const int MAX_LEVEL = 3;
    int upgradeCost() const { return m_baseStats[m_level].cost * (1 << (m_level - 1)); }
    int sellValue() const { return static_cast<int>(m_totalInvested / 2.0); }
    bool upgrade();
    QString name() const;

    int cost() const { return m_baseStats[m_level].cost; }
    double damage() const { return m_baseStats[m_level].damage; }
    double range() const { return m_baseStats[m_level].range; }
    double attackSpeed() const { return m_baseStats[m_level].attackSpeed; }
    QColor color() const { return m_baseStats[m_level].color; }
    int penetration() const { return m_baseStats[m_level].penetration; }
    int shotCount() const { return m_baseStats[m_level].shotCount; }
    int spreadAngle() const { return m_baseStats[m_level].spreadAngle; }
    int waveCount() const { return m_baseStats[m_level].waveCount; }
    double waveDelay() const { return m_baseStats[m_level].waveDelay; }
    const std::vector<std::unique_ptr<Marker>>& markers() const { return m_markerTemplates[m_level]; }

protected:
    Tower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY);

    virtual void drawBody(QPainter& p, const QPointF& center, double radius) const = 0;

    TowerType m_type;
    std::array<TowerStats, 4> m_baseStats;
    std::array<std::vector<std::unique_ptr<Marker>>, 4> m_markerTemplates;

    int m_gridX, m_gridY;
    double m_cellSize;
    int m_level = 1;
    int m_totalInvested = 0;

public:
    void setPriorityEnemy(class Enemy* e) { m_priorityEnemy = e; m_priorityObstacle = nullptr; }
    class Enemy* priorityEnemy() const { return m_priorityEnemy; }
    void setPriorityObstacle(class Obstacle* o) { m_priorityObstacle = o; m_priorityEnemy = nullptr; }
    class Obstacle* priorityObstacle() const { return m_priorityObstacle; }

protected:
    Obstacle* findObstacleTarget() const;
    double distTo(const Obstacle& o) const;
    std::vector<std::unique_ptr<Marker>> cloneMarkers() const;

    class Enemy* m_priorityEnemy = nullptr;
    class Obstacle* m_priorityObstacle = nullptr;
};

#endif