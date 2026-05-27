#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPointF>
#include <QPainter>
#include <QColor>

class Obstacle {
public:
    virtual ~Obstacle() = default;

    void update(double dt);
    void takeDamage(double dmg);
    void draw(QPainter* p) const;

    bool isDead() const { return m_hp <= 0; }
    bool isActive() const { return !isDead() && !m_destroying && !m_destroyed; }
    bool isDestroying() const { return m_destroying; }
    bool isDestroyed() const { return m_destroyed; }

    int gridX() const { return m_gridX; }
    int gridY() const { return m_gridY; }
    QPointF pos() const { return m_pos; }
    int reward() const { return m_reward; }
    int radius() const { return m_radius; }

    void startDestruction();

protected:
    Obstacle(int gridX, int gridY, const QPointF& pos, double maxHp, int reward, const QColor& color, int radius);

    virtual void drawBody(QPainter* p, const QPointF& center, int radius) const = 0;

    double m_hp;
    double m_maxHp;
    int m_gridX;
    int m_gridY;
    QPointF m_pos;
    int m_reward;
    QColor m_color;
    int m_radius;

    bool m_destroying;
    bool m_destroyed;
    double m_destroyTimer;
    static constexpr double DESTROY_DURATION = 0.5;
};

#endif