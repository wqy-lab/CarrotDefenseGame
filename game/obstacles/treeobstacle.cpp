#include "treeobstacle.h"
#include "../config/datamanager.h"

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
    }
}