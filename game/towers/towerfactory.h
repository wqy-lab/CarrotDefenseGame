#ifndef TOWERFACTORY_H
#define TOWERFACTORY_H

#include <memory>
#include <QPointF>
#include "tower.h"

class Tower;

std::unique_ptr<Tower> createTower(TowerType type, int gridX, int gridY,
                                    double cellSize, double offsetX, double offsetY);

#endif