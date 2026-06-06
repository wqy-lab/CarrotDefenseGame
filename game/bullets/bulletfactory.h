#ifndef BULLETFACTORY_H
#define BULLETFACTORY_H

#include <memory>
#include <QPointF>
#include <QColor>
#include <vector>
#include <memory>
#include "../markers/marker.h"
#include "../towers/tower.h"

class Bullet;

std::unique_ptr<Bullet> createBullet(TowerType type, const QPointF& start,
                                      const QPointF& direction, double damage,
                                      double splashRadius,
                                      const QColor& color,
                                      std::vector<std::unique_ptr<Marker>> markers,
                                      int penetration = 0);

#endif