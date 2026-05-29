#ifndef CELLENTITIES_H
#define CELLENTITIES_H

#include <vector>
class Enemy;
class Obstacle;

struct CellEntities {
    std::vector<Enemy*> enemies;
    std::vector<Obstacle*> obstacles;
};

#endif