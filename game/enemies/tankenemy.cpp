#include "tankenemy.h"
#include "../config/datamanager.h"

TankEnemy::TankEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Tank), "tank") {}
