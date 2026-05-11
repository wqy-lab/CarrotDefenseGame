#include "fastenemy.h"

FastEnemy::FastEnemy(const std::vector<QPointF>& path)
    : Enemy(path, {30.0, 100.0, 8, 1, QColor(255, 165, 0), 8}) {}

EnemyStats FastEnemy::getStats() const {
    return {30.0, 100.0, 8, 1, QColor(255, 165, 0), 8};
}

void FastEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    p.drawEllipse(center, r, r);
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(center.x() - r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.drawEllipse(QPointF(center.x() + r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.setBrush(Qt::black);
    p.drawEllipse(QPointF(center.x() - r*0.25, center.y() - r*0.27), r*0.11, r*0.11);
    p.drawEllipse(QPointF(center.x() + r*0.35, center.y() - r*0.27), r*0.11, r*0.11);
}
