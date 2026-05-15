#include "tankenemy.h"
#include "../config/datamanager.h"

TankEnemy::TankEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Tank)) {}

EnemyStats TankEnemy::getStats() const {
    return DataManager::instance().getEnemyStats(EnemyType::Tank);
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
