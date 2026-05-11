#include "swarmenemy.h"

SwarmEnemy::SwarmEnemy(const std::vector<QPointF>& path)
    : Enemy(path, {15.0, 120.0, 3, 1, QColor(255, 200, 50), 6}) {}

EnemyStats SwarmEnemy::getStats() const {
    return {15.0, 120.0, 3, 1, QColor(255, 200, 50), 6};
}

void SwarmEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    QPolygonF tri;
    tri << QPointF(center.x(), center.y() - r) << QPointF(center.x() + r, center.y() + r) << QPointF(center.x() - r, center.y() + r);
    p.drawPolygon(tri);
}
