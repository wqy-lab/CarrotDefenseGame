#include "tankenemy.h"

TankEnemy::TankEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Tank)) {}

void TankEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    p.drawEllipse(center, r, r);
    drawFace(p, center, r);
}
