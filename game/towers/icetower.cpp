#include "icetower.h"
#include "../config/datamanager.h"
#include <cmath>

IceTower::IceTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Ice, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void IceTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
    QString path = QString("assets/towers/ice_lv%1.png").arg(level());
    const QPixmap& tex = DataManager::instance().getTexture(path);

    if (!tex.isNull()) {
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), tex);
        return;
    }

    p.setBrush(color());
    p.setPen(QPen(QColor(60, 140, 220), 2));
    QPolygonF poly;
    for (int i = 0; i < 4; ++i) {
        double a = -3.14159/4 + 3.14159*2*i/4.0;
        poly << QPointF(center.x() + r*cos(a), center.y() + r*sin(a));
    }
    p.drawPolygon(poly);
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(QPointF(center.x() - r*0.5, center.y()), QPointF(center.x() + r*0.5, center.y()));
    p.drawLine(QPointF(center.x(), center.y() - r*0.5), QPointF(center.x(), center.y() + r*0.5));
    p.drawLine(QPointF(center.x() - r*0.35, center.y() - r*0.35), QPointF(center.x() + r*0.35, center.y() + r*0.35));
    p.drawLine(QPointF(center.x() + r*0.35, center.y() - r*0.35), QPointF(center.x() - r*0.35, center.y() + r*0.35));
}