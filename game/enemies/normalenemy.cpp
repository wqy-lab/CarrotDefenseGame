#include "normalenemy.h"
#include "../config/datamanager.h"

NormalEnemy::NormalEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Normal), "normal") {}
