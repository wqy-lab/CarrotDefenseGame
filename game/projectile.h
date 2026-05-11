#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QPointF>
#include <QPainter>
#include <QColor>

class Projectile {
public:
    Projectile(QPointF start, QPointF target, double damage,
               double splashRadius, double slowFactor, double slowDuration,
               QColor color, double speed = 600.0,
               double poisonDps = 0, double poisonDuration = 0,
               int chainCount = 0);

    void update(double dt);
    bool hasHit() const { return m_hit; }
    bool isActive() const { return !m_hit; }

    QPointF targetPos() const { return m_target; }
    double damage() const { return m_damage; }
    double splashRadius() const { return m_splashRadius; }
    double slowFactor() const { return m_slowFactor; }
    double slowDuration() const { return m_slowDuration; }
    double poisonDps() const { return m_poisonDps; }
    double poisonDuration() const { return m_poisonDuration; }
    int chainCount() const { return m_chainCount; }
    QPointF pos() const { return m_pos; }
    QColor color() const { return m_color; }

    void draw(QPainter& p) const;

private:
    QPointF m_pos, m_target;
    double m_damage, m_splashRadius, m_slowFactor, m_slowDuration;
    double m_poisonDps, m_poisonDuration;
    int m_chainCount;
    QColor m_color;
    double m_speed;
    bool m_hit;
};

#endif // PROJECTILE_H
