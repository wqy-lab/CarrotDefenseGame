#include "arrowtower.h"
#include "../config/datamanager.h"

ArrowTower::ArrowTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Arrow, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void ArrowTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
    QString path = QString("assets/towers/arrow_lv%1.png").arg(level());
    const QPixmap& tex = DataManager::instance().getTexture(path);

    if (!tex.isNull()) {
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), tex);
    }
}