#ifndef BULLETFACTORY_H
#define BULLETFACTORY_H

#include <memory>
#include <QPointF>
#include <QColor>
#include <vector>
#include <memory>
#include "../markers/marker.h"

class Bullet;

enum class BulletType {
    Arrow,
    Cannon,
    Ice,
    Poison,
    Lightning
};

std::unique_ptr<Bullet> createBullet(BulletType type, const QPointF& start,
                                      const QPointF& target, double damage,
                                      double splashRadius,
                                      const QColor& color,
                                      std::vector<std::unique_ptr<Marker>> markers);

#endif