#include "projectile.h"
#include <QtMath>

Projectile::Projectile(QPointF start, QPointF target, double damage,
                       double splashRadius, double slowFactor, double slowDuration,
                       QColor color, double speed,
                       double poisonDps, double poisonDuration, int chainCount)
    : m_pos(start), m_target(target)
    , m_damage(damage), m_splashRadius(splashRadius)
    , m_slowFactor(slowFactor), m_slowDuration(slowDuration)
    , m_poisonDps(poisonDps), m_poisonDuration(poisonDuration)
    , m_chainCount(chainCount)
    , m_color(color), m_speed(speed), m_hit(false)
{}

void Projectile::update(double dt)
{
    if (m_hit) return;

    QPointF dir = m_target - m_pos;
    double dist = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
    double moveDist = m_speed * dt;

    if (moveDist >= dist) {
        m_pos = m_target;
        m_hit = true;
    } else {
        dir /= dist;
        m_pos += dir * moveDist;
    }
}

void Projectile::draw(QPainter& p) const
{
    if (m_hit) return;
    p.setPen(Qt::NoPen);
    p.setBrush(m_color);
    p.drawEllipse(QPointF(m_pos), 4, 4);

    // Trail
    p.setBrush(QColor(m_color.red(), m_color.green(), m_color.blue(), 100));
    p.drawEllipse(QPointF(m_pos), 6, 6);
}
