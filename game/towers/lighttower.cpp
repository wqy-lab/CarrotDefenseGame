#include "lighttower.h"

LightningTower::LightningTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Lightning, gridX, gridY, cellSize, offsetX, offsetY)
{
    m_stats = { 90, 18.0, 3.5, 1.8, 0, 1.0, 0, 0, 0, 3, QColor(220, 200, 60) };
}

TowerStats LightningTower::getStats() const {
    return { 90, 18.0, 3.5, 1.8, 0, 1.0, 0, 0, 0, 3, QColor(220, 200, 60) };
}

void LightningTower::drawBody(QPainter& p, const QPointF& center, double r) const {
    p.setBrush(m_stats.color);
    p.setPen(QPen(QColor(180, 160, 30), 2));
    QPolygonF poly;
    for (int i = 0; i < 10; ++i) {
        double a = -3.14159/2 + 3.14159*2*i/10.0;
        double ra = (i % 2 == 0) ? r : r*0.6;
        poly << QPointF(center.x() + ra*cos(a), center.y() + ra*sin(a));
    }
    p.drawPolygon(poly);
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(QPointF(center.x(), center.y() - r*0.8), QPointF(center.x() + r*0.15, center.y()));
    p.drawLine(QPointF(center.x() + r*0.15, center.y()), QPointF(center.x() - r*0.1, center.y() + r*0.4));
    p.drawLine(QPointF(center.x() - r*0.1, center.y() + r*0.4), QPointF(center.x() + r*0.05, center.y() + r*0.5));
}