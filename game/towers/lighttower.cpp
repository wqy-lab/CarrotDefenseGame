#include "lighttower.h"
#include "../config/datamanager.h"

LightningTower::LightningTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Lightning, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void LightningTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
    QString path = QString("assets/towers/lightning_lv%1.png").arg(level());
    const QPixmap& tex = DataManager::instance().getTexture(path);

    if (!tex.isNull()) {
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), tex);
    }
}