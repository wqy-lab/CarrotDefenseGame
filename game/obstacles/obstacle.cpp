#include "obstacle.h"

Obstacle::Obstacle(int gridX, int gridY, int gridW, int gridH, double maxHp, int reward, const QColor& color, int radius)
    : m_hp(maxHp)
    , m_maxHp(maxHp)
    , m_gridX(gridX)
    , m_gridY(gridY)
    , m_gridW(gridW)
    , m_gridH(gridH)
    , m_reward(reward)
    , m_color(color)
    , m_radius(radius)
    , m_destroying(false)
    , m_destroyed(false)
    , m_destroyTimer(0.0)
{}

void Obstacle::update(double dt) {
    if (m_destroying) {
        m_destroyTimer += dt;
        if (m_destroyTimer >= DESTROY_DURATION) {
            m_destroying = false;
            m_destroyed = true;
        }
    }
}

void Obstacle::takeDamage(double dmg) {
    if (m_destroying || m_destroyed) return;
    m_hp -= dmg;
    if (m_hp <= 0) {
        m_hp = 0;
        startDestruction();
    }
}

void Obstacle::startDestruction() {
    m_destroying = true;
    m_destroyTimer = 0.0;
}

void Obstacle::draw(QPainter* p, double cellSize, double offsetX, double offsetY) const {
    if (m_destroyed) return;

    double centerX = offsetX + (m_gridX + m_gridW / 2.0) * cellSize;
    double centerY = offsetY + (m_gridY + m_gridH / 2.0) * cellSize;
    QPointF center(centerX, centerY);
    int r = m_radius;

    if (m_destroying) {
        double progress = m_destroyTimer / DESTROY_DURATION;
        double alpha = 1.0 - progress;
        double scale = 1.0 - progress * 0.5;
        int animR = static_cast<int>(r * scale);

        QColor col = m_color;
        col.setAlphaF(alpha);
        p->setPen(Qt::NoPen);
        p->setBrush(col);
        p->drawEllipse(center, animR, animR);
    } else {
        int barW = r * 3;
        int barH = 4;
        p->fillRect(QRectF(center.x() - barW / 2.0, center.y() - r - 10, barW, barH), QColor(40, 40, 40));
        double hpRatio = m_hp / m_maxHp;
        QColor hpCol = hpRatio > 0.5 ? QColor(76, 175, 80) : hpRatio > 0.25 ? QColor(255, 193, 7) : QColor(244, 67, 54);
        p->fillRect(QRectF(center.x() - barW / 2.0, center.y() - r - 10, barW * hpRatio, barH), hpCol);

        p->setPen(Qt::NoPen);
        p->setBrush(m_color);
        drawBody(p, center, r);
    }
}