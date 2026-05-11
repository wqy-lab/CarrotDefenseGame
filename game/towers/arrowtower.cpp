#include "arrowtower.h"

ArrowTower::ArrowTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Arrow, gridX, gridY, cellSize, offsetX, offsetY)
{
    m_stats = { 40, 20.0, 3.0, 0.6, 0, 1.0, 0, 0, 0, 0, QColor(139, 195, 74) };
}

TowerStats ArrowTower::getStats() const {
    return { 40, 20.0, 3.0, 0.6, 0, 1.0, 0, 0, 0, 0, QColor(139, 195, 74) };
}

void ArrowTower::drawBody(QPainter& p, const QPointF& center, double r) const {
    p.setBrush(m_stats.color);
    p.setPen(QPen(QColor(100,140,50), 1.5));
    QPolygonF poly;
    for (int i = 0; i < 5; ++i) {
        double a = -3.14159/2 + 3.14159*2*i/5.0;
        poly << QPointF(center.x() + r*cos(a), center.y() + r*sin(a));
    }
    p.drawPolygon(poly);
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(QPointF(center.x(), center.y() - r*0.6), QPointF(center.x(), center.y() + r*0.3));
    p.drawLine(QPointF(center.x() - r*0.4, center.y() - r*0.1), QPointF(center.x(), center.y() + r*0.3));
    p.drawLine(QPointF(center.x() + r*0.4, center.y() - r*0.1), QPointF(center.x(), center.y() + r*0.3));
}