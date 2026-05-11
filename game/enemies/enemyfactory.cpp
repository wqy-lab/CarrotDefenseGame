#include "enemyfactory.h"
#include "normalenemy.h"
#include "fastenemy.h"
#include "tankenemy.h"
#include "bossenemy.h"
#include "swarmenemy.h"

std::unique_ptr<Enemy> createEnemy(EnemyType type, const std::vector<QPointF>& path)
{
    switch (type) {
    case EnemyType::Normal:  return std::make_unique<NormalEnemy>(path);
    case EnemyType::Fast:    return std::make_unique<FastEnemy>(path);
    case EnemyType::Tank:    return std::make_unique<TankEnemy>(path);
    case EnemyType::Boss:    return std::make_unique<BossEnemy>(path);
    case EnemyType::Swarm:   return std::make_unique<SwarmEnemy>(path);
    }
    return std::make_unique<NormalEnemy>(path);
}
