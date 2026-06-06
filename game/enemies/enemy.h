#ifndef ENEMY_H
#define ENEMY_H

#include <QPointF>
#include <QPainter>
#include <vector>
#include <utility>
#include <map>
#include <memory>
#include "../markers/marker.h"

struct EnemyStats {
    double maxHp;
    double speed;
    int reward;
    int damage;
    QColor color;
    int radius;
};

class Enemy {
public:
    virtual ~Enemy() = default;

    void update(double dt);
    void takeDamage(double dmg);

    bool isDead() const { return m_hp <= 0; }
    bool reachedEnd() const { return m_reachedEnd; }
    bool isActive() const { return !m_reachedEnd && !isDead(); }

    void updatePath(const std::vector<QPointF>& newPath);

    QPointF pos(double cellSize, double offsetX, double offsetY) const;
    QPointF gridPos() const { return m_gridPos; }
    double hp() const { return m_hp; }
    double maxHp() const { return m_stats.maxHp; }
    int reward() const { return m_stats.reward; }
    int damage() const { return m_stats.damage; }
    double radius() const { return m_stats.radius; }
    QColor color() const { return m_stats.color; }
    double speed() const;  // 查询所有Marker，返回最终速度

    EnemyStats getStats() const { return m_stats; }

    void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const;

    QString textureTag() const { return m_textureTag; }

    // Marker management
    void addMarker(std::unique_ptr<Marker> marker);
    void updateMarkers(double dt);
    void removeInactiveMarkers();

protected:
    Enemy(const std::vector<QPointF>& path, EnemyStats stats,
          const QString& textureTag = QString());

    EnemyStats m_stats;

    double m_hp;
    QPointF m_gridPos;
    std::vector<QPointF> m_path;
    int m_pathIndex;
    bool m_reachedEnd;

    bool m_goldAwarded;
    QString m_textureTag;

    std::map<QString, std::vector<std::unique_ptr<Marker>>> m_markers;

public:
    bool consumeReward();
    int pathIndex() const { return m_pathIndex; }
};

#endif