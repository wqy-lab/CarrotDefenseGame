#include "treeobstacle.h"
#include "../config/datamanager.h"
#include <QPainterPath>
#include <QColor>

TreeObstacle::TreeObstacle(int gridX, int gridY, int gridW, int gridH)
    : Obstacle(gridX, gridY, gridW, gridH, 100.0, 15, QColor(76, 175, 80), 18)
{}

void TreeObstacle::drawBody(QPainter* p, const QPointF& center, int radius) const
{
    const QPixmap& tex = DataManager::instance().getTexture(
        "assets/obstacles/obstacle_tree.png");

    if (!tex.isNull()) {
        QRectF target(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
        p->drawPixmap(target.toRect(), tex);
        return;
    }

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