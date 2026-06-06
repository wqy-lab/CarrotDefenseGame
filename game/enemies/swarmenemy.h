#ifndef SWARMENEMY_H
#define SWARMENEMY_H

#include <vector>
#include <QPointF>
#include "enemy.h"

class SwarmEnemy : public Enemy {
public:
    SwarmEnemy(const std::vector<QPointF>& path);
};

#endif
