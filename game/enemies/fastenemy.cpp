#include "fastenemy.h"
#include "../config/datamanager.h"

FastEnemy::FastEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Fast), "fast") {}

void FastEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    p.drawEllipse(center, r, r);
    drawFace(p, center, r);
}
