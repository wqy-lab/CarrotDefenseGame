#include "normalenemy.h"

NormalEnemy::NormalEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Normal)) {}

void NormalEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    p.drawEllipse(center, r, r);
    drawFace(p, center, r);
}
