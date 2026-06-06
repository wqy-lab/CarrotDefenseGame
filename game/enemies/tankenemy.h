#ifndef TANKENEMY_H
#define TANKENEMY_H

#include <vector>
#include <QPointF>
#include "enemy.h"

class TankEnemy : public Enemy {
public:
    TankEnemy(const std::vector<QPointF>& path);
};

#endif
