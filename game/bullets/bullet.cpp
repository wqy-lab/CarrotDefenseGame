#include "bullet.h"
#include "../obstacles/obstacle.h"
#include "../enemies/enemy.h"
#include <QtMath>

Bullet::Bullet(const QPointF& start, const QPointF& target, double damage,
               double splashRadius, double slowFactor, double slowDuration,
               double poisonDps, double poisonDuration, int chainCount, const QColor& color)
    : m_pos(start)
    , m_targetPos(target)
    , m_startPos(start)
    , m_maxDistance(0.0)
    , m_traveledDistance(0.0)
    , m_speed(400.0)
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
}

void Bullet::update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    if (!m_active) return;

    // Determine which grid cell the bullet is currently in
    int bulletCellX = static_cast<int>(m_pos.x());
    int bulletCellY = static_cast<int>(m_pos.y());

    QPointF dir = m_targetPos - m_pos;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (dist > 0) dir /= dist;

    // Speed in grid units per second (original 400 pixels/second converted to grids)
    double moveDist = (m_speed / m_cellSize) * dt;

    // Move bullet
    m_pos += dir * moveDist;
    m_traveledDistance += moveDist;

    // Check max range (maxDistance was stored in grids)
    if (m_maxDistance > 0 && m_traveledDistance > m_maxDistance) {
        m_active = false;
        return;
    }

    // Check which cell the bullet is in after moving
    int newCellX = static_cast<int>(m_pos.x());
    int newCellY = static_cast<int>(m_pos.y());

    // Check obstacle collision first (obstacle takes priority)
    for (Obstacle* obs : cell.obstacles) {
        if (!obs->isActive()) continue;
        if (newCellX >= obs->gridX() && newCellX < obs->gridX() + obs->gridWidth() &&
            newCellY >= obs->gridY() && newCellY < obs->gridY() + obs->gridHeight()) {
            m_hit = true;
            onObstacleHit(obs);
            m_active = false;
            return;
        }
    }

    // Check enemy collision
    for (Enemy* e : cell.enemies) {
        if (!e->isActive()) continue;
        // Both m_pos and e->gridPos() are in grid units
        QPointF d = e->gridPos() - m_pos;
        double d2 = d.x()*d.x() + d.y()*d.y();
        double hitRadius = static_cast<double>(e->radius()) / m_cellSize * 1.5;
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