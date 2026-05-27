#include "statueobstacle.h"

StatueObstacle::StatueObstacle(int gridX, int gridY, const QPointF& pos)
    : Obstacle(gridX, gridY, pos, 200.0, 30, QColor(189, 189, 189), 16)
{}

void StatueObstacle::drawBody(QPainter* p, const QPointF& center, int radius) const {
    p->setBrush(QColor(189, 189, 189));
    p->drawRect(QRectF(center.x() - radius * 0.4f, center.y() - radius * 0.5f,
                       radius * 0.8f, radius * 1.1f));
    p->setBrush(QColor(150, 150, 150));
    p->drawEllipse(center, radius * 0.35f, radius * 0.35f);
}