#include "suntower.h"
#include <cmath>

SunTower::SunTower(int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : MeleeTower(TowerType::Sun, gridX, gridY, cellSize, offsetX, offsetY)
{
}

void SunTower::drawBody(QPainter& p, const QPointF& center, double r) const {
    p.setBrush(m_stats.color);
    p.setPen(QPen(QColor(200, 150, 30), 2));
    p.drawEllipse(center, r, r);
    p.setPen(QPen(Qt::white, 2));
    for (int i = 0; i < 8; ++i) {
        double a = 3.14159 * i / 4.0;
        p.drawLine(QPointF(center.x() + r*1.2*cos(a), center.y() + r*1.2*sin(a)),
                   QPointF(center.x() + r*0.8*cos(a), center.y() + r*0.8*sin(a)));
    }
}