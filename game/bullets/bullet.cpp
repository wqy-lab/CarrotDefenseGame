#include "bullet.h"
#include "../enemies/enemy.h"
#include <QtMath>

Bullet::Bullet(const QPointF& start, const QPointF& target, double damage,
               double splashRadius, double slowFactor, double slowDuration,
               double poisonDps, double poisonDuration, int chainCount, const QColor& color)
    : m_pos(start), m_targetPos(target), m_damage(damage)
    , m_splashRadius(splashRadius), m_slowFactor(slowFactor), m_slowDuration(slowDuration)
    , m_poisonDps(poisonDps), m_poisonDuration(poisonDuration)
    , m_chainCount(chainCount), m_color(color), m_active(true)
    , m_speed(400.0)
{
}

void Bullet::update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (!m_active) return;

    QPointF dir = m_targetPos - m_pos;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    double moveDist = m_speed * dt;

    if (moveDist >= dist) {
        m_pos = m_targetPos;
        for (auto& e : enemies) {
            if (!e->isActive()) continue;
            QPointF d = e->pos() - m_pos;
            double d2 = d.x()*d.x() + d.y()*d.y();
            if (d2 <= 400) {
                onHit(e.get(), enemies);
                break;
            }
        }
        m_active = false;
    } else {
        dir /= dist;
        m_pos += dir * moveDist;
    }
}

void Bullet::draw(QPainter& p) const
{
    if (!m_active) return;
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(m_color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(m_pos, 5, 5);
}