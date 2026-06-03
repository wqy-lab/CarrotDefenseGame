#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPointF>
#include <QPainter>
#include <QColor>
#include <QPixmap>

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
    int gridWidth() const { return m_gridW; }
    int gridHeight() const { return m_gridH; }
    QPointF pos() const { return m_pos; }
    int reward() const { return m_reward; }
    int radius() const { return m_radius; }
    int cellSize() const { return m_cellSize; }
    void setCellSize(int cs) { m_cellSize = cs; }
    void setPosition(const QPointF& pos) { m_pos = pos; }

    void startDestruction();

protected:
    Obstacle(int gridX, int gridY, int gridW, int gridH, const QPointF& pos,
             double maxHp, int reward, const QColor& color, int radius,
             const QPixmap& texture = QPixmap());

    virtual void drawBody(QPainter* p, const QPointF& center, int radius) const = 0;

    double m_hp;
    double m_maxHp;
    int m_gridX;
    int m_gridY;
    int m_gridW;
    int m_gridH;
    QPointF m_pos;
    int m_reward;
    QColor m_color;
    int m_radius;
    QPixmap m_texture;

    bool m_destroying;
    bool m_destroyed;
    double m_destroyTimer;
    static constexpr double DESTROY_DURATION = 0.5;
    int m_cellSize;
};

#endif