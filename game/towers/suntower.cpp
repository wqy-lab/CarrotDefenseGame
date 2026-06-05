#include "suntower.h"
#include "../config/datamanager.h"
#include <cmath>

SunTower::SunTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : MeleeTower(TowerType::Sun, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void SunTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
    p.setRenderHint(QPainter::Antialiasing, true);

    if (m_cooldown > 0) {
        double progress = 1.0 - m_cooldown / attackSpeed();
        double maxRadius = range() * m_cellSize;
        double currentRadius = maxRadius * progress;
        int alpha = static_cast<int>(150 * (1.0 - progress) + 20);
        if (alpha > 0 && currentRadius > 0) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(255, 200, 50, alpha));
            p.drawEllipse(center, currentRadius, currentRadius);
        }
    }

    QString path = QString("assets/towers/sun_lv%1.png").arg(level());
    const QPixmap& tex = DataManager::instance().getTexture(path);

    if (!tex.isNull()) {
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), tex);
        return;
    }

    p.setBrush(color());
    p.setPen(QPen(QColor(200, 150, 30), 2));
    p.drawEllipse(center, r, r);
    p.setPen(QPen(Qt::white, 2));
    for (int i = 0; i < 8; ++i) {
        double a = 3.14159 * i / 4.0;
        p.drawLine(QPointF(center.x() + r*1.2*cos(a), center.y() + r*1.2*sin(a)),
                   QPointF(center.x() + r*0.8*cos(a), center.y() + r*0.8*sin(a)));
    }
}