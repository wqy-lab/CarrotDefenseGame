#include "bossenemy.h"
#include <cmath>

BossEnemy::BossEnemy(const std::vector<QPointF>& path)
    : Enemy(path, {500.0, 18.0, 50, 3, QColor(200, 50, 50), 18}) {}

EnemyStats BossEnemy::getStats() const {
    return {500.0, 18.0, 50, 3, QColor(200, 50, 50), 18};
}

void BossEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    QPolygonF hex;
    for (int i = 0; i < 6; ++i) {
        double a = 3.14159 * i / 3.0;
        hex << QPointF(center.x() + r * std::cos(a), center.y() + r * std::sin(a));
    }
    p.drawPolygon(hex);
    p.setBrush(QColor(255, 215, 0));
    p.drawEllipse(QPointF(center.x(), center.y() - r*0.7), r*0.2, r*0.2);
}
