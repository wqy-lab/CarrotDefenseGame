#ifndef ENEMY_H
#define ENEMY_H

#include <QPointF>
#include <QPainter>
#include <vector>
#include <utility>

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
    void applySlow(double factor, double duration);
    void applyPoison(double dps, double duration);
    void takeDamage(double dmg);

    bool isDead() const { return m_hp <= 0; }
    bool reachedEnd() const { return m_reachedEnd; }
    bool isActive() const { return !m_reachedEnd && !isDead(); }

    void updatePath(const std::vector<QPointF>& newPath);

    QPointF pos() const { return m_pos; }
    double hp() const { return m_hp; }
    double maxHp() const { return getStats().maxHp; }
    int reward() const { return getStats().reward; }
    int damage() const { return getStats().damage; }
    double radius() const { return getStats().radius; }
    QColor color() const { return getStats().color; }
    double speed() const { return getStats().speed * m_slowFactor; }

    void draw(QPainter& p) const;

protected:
    Enemy(const std::vector<QPointF>& path, EnemyStats stats);

    virtual EnemyStats getStats() const = 0;
    virtual void drawBody(QPainter& p, const QPointF& center, int radius) const = 0;

    double m_hp;
    QPointF m_pos;
    std::vector<QPointF> m_path;
    int m_pathIndex;
    bool m_reachedEnd;

    double m_slowFactor;
    double m_slowTimer;
    double m_poisonDps;
    double m_poisonTimer;
    bool m_goldAwarded;

public:
    bool consumeReward();  // Returns true if reward not yet claimed, marks as claimed
    int pathIndex() const { return m_pathIndex; }
};

#endif