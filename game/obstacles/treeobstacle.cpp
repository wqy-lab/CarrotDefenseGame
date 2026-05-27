#include "treeobstacle.h"
#include <QPainterPath>
#include <QColor>

TreeObstacle::TreeObstacle(int gridX, int gridY, int gridW, int gridH, const QPointF& pos)
    : Obstacle(gridX, gridY, gridW, gridH, pos, 100.0, 15, QColor(76, 175, 80), 18)
{}

void TreeObstacle::drawBody(QPainter* p, const QPointF& center, int radius) const {
    QPainterPath path;
    path.moveTo(center.x(), center.y() - radius);
    path.lineTo(center.x() - radius, center.y() + radius * 0.4);
    path.lineTo(center.x() + radius, center.y() + radius * 0.4);
    path.closeSubpath();
    p->drawPath(path);

    p->fillRect(QRectF(center.x() - radius * 0.15f, center.y() + radius * 0.3f,
                       radius * 0.3f, radius * 0.5f),
                QColor(101, 67, 33));
}