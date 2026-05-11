#ifndef ENEMYFACTORY_H
#define ENEMYFACTORY_H

#include <memory>
#include <vector>
#include <QPointF>

class Enemy;

enum class EnemyType {
    Normal,
    Fast,
    Tank,
    Boss,
    Swarm
};

std::unique_ptr<Enemy> createEnemy(EnemyType type, const std::vector<QPointF>& path);

#endif
