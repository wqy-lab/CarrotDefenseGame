#include "canntower.h"
#include "../config/datamanager.h"

CannonTower::CannonTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Cannon, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void CannonTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
    QString path = QString("assets/towers/cannon_lv%1.png").arg(level());
    const QPixmap& tex = DataManager::instance().getTexture(path);

    if (!tex.isNull()) {
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), tex);
        return;
    }

    p.setBrush(color());
    p.setPen(QPen(QColor(200,120,0), 2));
    p.drawEllipse(center, r*0.9, r*0.9);
    p.setBrush(QColor(180,100,0));
    p.setPen(QPen(Qt::black, 1.5));
    p.drawRect(QRectF(center.x() - r*0.2, center.y() - r, r*0.4, r*0.6));
}