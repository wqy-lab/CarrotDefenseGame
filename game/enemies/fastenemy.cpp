#include "fastenemy.h"
#include "../config/datamanager.h"

FastEnemy::FastEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Fast), "fast") {}
