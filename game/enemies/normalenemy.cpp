#include "normalenemy.h"

NormalEnemy::NormalEnemy(const std::vector<QPointF>& path)
    : Enemy(path, {50.0, 50.0, 5, 1, QColor(220, 60, 60), 10}) {}

EnemyStats NormalEnemy::getStats() const {
    return {50.0, 50.0, 5, 1, QColor(220, 60, 60), 10};
}

void NormalEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    p.drawEllipse(center, r, r);
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(center.x() - r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.drawEllipse(QPointF(center.x() + r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.setBrush(Qt::black);
    p.drawEllipse(QPointF(center.x() - r*0.25, center.y() - r*0.27), r*0.11, r*0.11);
    p.drawEllipse(QPointF(center.x() + r*0.35, center.y() - r*0.27), r*0.11, r*0.11);
}
