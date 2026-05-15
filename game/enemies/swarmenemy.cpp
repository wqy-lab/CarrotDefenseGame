#include "swarmenemy.h"
#include "../config/datamanager.h"

SwarmEnemy::SwarmEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Swarm)) {}

EnemyStats SwarmEnemy::getStats() const {
    return DataManager::instance().getEnemyStats(EnemyType::Swarm);
}

void SwarmEnemy::drawBody(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(color());
    QPolygonF tri;
    tri << QPointF(center.x(), center.y() - r) << QPointF(center.x() + r, center.y() + r) << QPointF(center.x() - r, center.y() + r);
    p.drawPolygon(tri);
}
