#include "bullet.h"
#include "../obstacles/obstacle.h"
#include "../enemies/enemy.h"
#include <QtMath>

Bullet::Bullet(const QPointF& start, const QPointF& target, double damage,
               double splashRadius, double slowFactor, double slowDuration,
               double poisonDps, double poisonDuration, int chainCount,
               const QColor& color, const QPixmap& texture)
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
    , m_texture(texture)
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
    int bulletCellX = static_cast<int>((m_pos.x() - m_offsetX) / m_cellSize);
    int bulletCellY = static_cast<int>((m_pos.y() - m_offsetY) / m_cellSize);

    QPointF dir = m_targetPos - m_pos;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (dist > 0) dir /= dist;

    double moveDist = m_speed * dt;

    // Move bullet
    m_pos += dir * moveDist;
    m_traveledDistance += moveDist;

    // Check max range
    if (m_maxDistance > 0 && m_traveledDistance > m_maxDistance) {
        m_active = false;
        return;
    }

    // Check which cell the bullet is in after moving
    int newCellX = static_cast<int>((m_pos.x() - m_offsetX) / m_cellSize);
    int newCellY = static_cast<int>((m_pos.y() - m_offsetY) / m_cellSize);

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
        QPointF d = e->pos() - m_pos;
        double d2 = d.x()*d.x() + d.y()*d.y();
        double hitRadius = static_cast<double>(e->radius()) * 1.5;
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

void Bullet::draw(QPainter& p) const
{
    if (!m_active) return;
    p.setRenderHint(QPainter::Antialiasing);

    if (!m_texture.isNull()) {
        p.drawPixmap(QRectF(m_pos.x() - 8, m_pos.y() - 8, 16, 16).toRect(), m_texture);
        return;
    }

    p.setBrush(m_color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(m_pos, 5, 5);
}