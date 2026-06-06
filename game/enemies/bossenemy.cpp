#include "bossenemy.h"
#include "../config/datamanager.h"

BossEnemy::BossEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Boss), "boss") {}
