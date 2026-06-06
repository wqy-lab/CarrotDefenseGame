#include "swarmenemy.h"
#include "../config/datamanager.h"

SwarmEnemy::SwarmEnemy(const std::vector<QPointF>& path)
    : Enemy(path, DataManager::instance().getEnemyStats(EnemyType::Swarm), "swarm") {}
