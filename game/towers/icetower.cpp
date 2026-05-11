#include "icetower.h"

IceTower::IceTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Ice, gridX, gridY, cellSize, offsetX, offsetY)
{
    m_stats = { 60, 12.0, 2.5, 1.0, 0, 0.5, 2.0, 0, 0, 0, QColor(100, 180, 255) };
}

TowerStats IceTower::getStats() const {
    return { 60, 12.0, 2.5, 1.0, 0, 0.5, 2.0, 0, 0, 0, QColor(100, 180, 255) };
}

void IceTower::drawBody(QPainter& p, const QPointF& center, double r) const {
    p.setBrush(m_stats.color);
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