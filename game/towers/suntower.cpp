#include "suntower.h"
#include "../config/datamanager.h"

SunTower::SunTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : MeleeTower(TowerType::Sun, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void SunTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
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
    }
}