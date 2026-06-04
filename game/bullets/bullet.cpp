#include "bullet.h"
#include "../obstacles/obstacle.h"
#include "../enemies/enemy.h"
#include <QtMath>

Bullet::Bullet(const QPointF& start, const QPointF& target, double damage, const QColor& color)
    : m_pos(start)
    , m_direction(0.0, 0.0)
    , m_startPos(start)
    , m_maxDistance(0.0)
    , m_traveledDistance(0.0)
    , m_speed(400.0 / 48.0)
    , m_damage(damage)
    , m_color(color)
    , m_active(true)
    , m_hit(false)
{
    QPointF dir = target - start;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (dist > 0) m_direction = dir / dist;
}

void Bullet::update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    if (!m_active) return;

    double moveDist = m_speed * dt;
    m_pos += m_direction * moveDist;
    m_traveledDistance += moveDist;

    if (m_maxDistance > 0 && m_traveledDistance > m_maxDistance) {
        m_active = false;
        return;
    }

    if (m_pos.x() < 0 || m_pos.x() >= m_gridCols ||
        m_pos.y() < 0 || m_pos.y() >= m_gridRows) {
        m_active = false;
        return;
    }

    for (Obstacle* obs : cell.obstacles) {
        if (!obs->isActive()) continue;
        if (m_pos.x() >= obs->gridX() && m_pos.x() < obs->gridX() + obs->gridWidth() &&
            m_pos.y() >= obs->gridY() && m_pos.y() < obs->gridY() + obs->gridHeight()) {
            m_hit = true;
            onObstacleHit(obs);
            m_active = false;
            return;
        }
    }

    for (Enemy* e : cell.enemies) {
        if (!e->isActive()) continue;
        QPointF d = e->gridPos() - m_pos;
        double d2 = d.x()*d.x() + d.y()*d.y();
        double hitRadius = 0.5;
        if (d2 <= hitRadius * hitRadius) {
            m_hit = true;
            onHit(e, enemies, cell);
            m_active = false;
            return;
        }
    }
}

void Bullet::onObstacleHit(Obstacle* obstacle) {
    obstacle->takeDamage(m_damage);
}

void Bullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(m_color);
    p.setPen(Qt::NoPen);
    QPointF pixelPos = QPointF(offsetX + m_pos.x() * cellSize,
                               offsetY + m_pos.y() * cellSize);
    p.drawEllipse(pixelPos, 5, 5);
}