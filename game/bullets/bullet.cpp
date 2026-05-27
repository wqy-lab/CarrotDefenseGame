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
{
}

void Bullet::update(double dt, CellEntities& cell)
{
    if (!m_active) return;

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

    // Check obstacle collision first (obstacle takes priority)
    for (Obstacle* obs : cell.obstacles) {
        if (!obs->isActive()) continue;
        QPointF d = obs->pos() - m_pos;
        double d2 = d.x()*d.x() + d.y()*d.y();
        double hitRadius = static_cast<double>(obs->radius()) * 1.5;
        if (d2 <= hitRadius * hitRadius) {
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
            onHit(e);
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
    p.setBrush(m_color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(m_pos, 5, 5);
}