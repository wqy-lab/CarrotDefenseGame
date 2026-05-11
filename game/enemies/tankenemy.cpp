#include "tankenemy.h"

TankEnemy::TankEnemy(const std::vector<QPointF>& path)
    : Enemy(path, {150.0, 28.0, 15, 1, QColor(100, 50, 150), 14}) {}

EnemyStats TankEnemy::getStats() const {
    return {150.0, 28.0, 15, 1, QColor(100, 50, 150), 14};
}

void TankEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    p.drawEllipse(center, r, r);
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(center.x() - r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.drawEllipse(QPointF(center.x() + r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.setBrush(Qt::black);
    p.drawEllipse(QPointF(center.x() - r*0.25, center.y() - r*0.27), r*0.11, r*0.11);
    p.drawEllipse(QPointF(center.x() + r*0.35, center.y() - r*0.27), r*0.11, r*0.11);
}
