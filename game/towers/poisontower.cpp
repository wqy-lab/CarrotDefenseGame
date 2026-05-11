#include "poisontower.h"

PoisonTower::PoisonTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : RemoteTower(TowerType::Poison, gridX, gridY, cellSize, offsetX, offsetY)
{
    m_stats = { 65, 8.0, 2.8, 1.2, 0, 1.0, 0, 25.0, 3.0, 0, QColor(120, 200, 80) };
}

TowerStats PoisonTower::getStats() const {
    return { 65, 8.0, 2.8, 1.2, 0, 1.0, 0, 25.0, 3.0, 0, QColor(120, 200, 80) };
}

void PoisonTower::drawBody(QPainter& p, const QPointF& center, double r) const {
    p.setBrush(m_stats.color);
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