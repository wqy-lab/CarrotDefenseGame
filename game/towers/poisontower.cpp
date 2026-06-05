#include "poisontower.h"
#include "../config/datamanager.h"
#include <cmath>

PoisonTower::PoisonTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Poison, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void PoisonTower::drawBody(QPainter& p, const QPointF& center, double r) const
{
    QString path = QString("assets/towers/poison_lv%1.png").arg(level());
    const QPixmap& tex = DataManager::instance().getTexture(path);

    if (!tex.isNull()) {
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), tex);
        return;
    }

    p.setBrush(color());
    p.setPen(QPen(QColor(80, 160, 50), 2));
    QPolygonF poly;
    for (int i = 0; i < 6; ++i) {
        double a = -3.14159/6 + 3.14159*2*i/6.0;
        poly << QPointF(center.x() + r*cos(a), center.y() + r*sin(a));
    }
    p.drawPolygon(poly);
    p.setPen(QPen(QColor(50, 100, 30), 2));
    p.drawEllipse(QPointF(center.x(), center.y() - r*0.15), r*0.15, r*0.15);
    p.drawEllipse(QPointF(center.x() - r*0.25, center.y() + r*0.2), r*0.1, r*0.12);
    p.drawEllipse(QPointF(center.x() + r*0.25, center.y() + r*0.2), r*0.1, r*0.12);
}