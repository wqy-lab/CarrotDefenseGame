#include "bullet.h"
#include "../obstacles/obstacle.h"
#include "../enemies/enemy.h"
#include <QtMath>

Bullet::Bullet(const QPointF& start, const QPointF& target, double damage,
               double splashRadius, double slowFactor, double slowDuration,
               double poisonDps, double poisonDuration, int chainCount, const QColor& color)
    : m_pos(start)
    , m_direction(0.0, 0.0)
    , m_startPos(start)
    , m_maxDistance(0.0)
    , m_traveledDistance(0.0)
    , m_speed(400.0 / 48.0)  // 400 pixels/sec → grids/sec
    , m_damage(damage)
    , m_splashRadius(splashRadius)
    , m_slowFactor(slowFactor)
    , m_slowDuration(slowDuration)
    , m_poisonDps(poisonDps)
    , m_poisonDuration(poisonDuration)
    , m_chainCount(chainCount)
    , m_color(color)
    , m_active(true)
    , m_hit(false)
    , m_cellSize(48.0)
    , m_offsetX(0.0)
    , m_offsetY(0.0)
{
    // Calculate direction from start to target
    QPointF dir = target - start;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (dist > 0) m_direction = dir / dist;
}

void Bullet::update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    if (!m_active) return;

    // Speed is stored as grids/sec directly
    double moveDist = m_speed * dt;

    // Move bullet in the fixed direction
    m_pos += m_direction * moveDist;
    m_traveledDistance += moveDist;

    // Check max range
    if (m_maxDistance > 0 && m_traveledDistance > m_maxDistance) {
        m_active = false;
        return;
    }

    // Check if bullet is out of grid bounds
    if (m_pos.x() < 0 || m_pos.x() >= m_gridCols ||
        m_pos.y() < 0 || m_pos.y() >= m_gridRows) {
        m_active = false;
        return;
    }

    // Check obstacle collision along the path
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

    // Check enemy collision
    for (Enemy* e : cell.enemies) {
        if (!e->isActive()) continue;
        QPointF d = e->gridPos() - m_pos;
        double d2 = d.x()*d.x() + d.y()*d.y();
        // hitRadius: 0.2 grids (~10px at cellSize=48) - tight collision
        double hitRadius = 0.2;
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
    QPointF pixelPos = QPointF(offsetX + m_pos.x() * cellSize + cellSize / 2.0,
                               offsetY + m_pos.y() * cellSize + cellSize / 2.0);
    p.drawEllipse(pixelPos, 5, 5);
}
