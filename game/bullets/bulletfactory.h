#ifndef BULLETFACTORY_H
#define BULLETFACTORY_H

#include <memory>
#include <QPointF>
#include <QColor>
#include <QPixmap>

class Bullet;

enum class BulletType {
    Normal
};

std::unique_ptr<Bullet> createBullet(BulletType type, const QPointF& start,
                                      const QPointF& target, double damage,
                                      double splashRadius = 0, double slowFactor = 1.0,
                                      double slowDuration = 0, double poisonDps = 0,
                                      double poisonDuration = 0, int chainCount = 0,
                                      const QColor& color = Qt::red,
                                      const QPixmap& texture = QPixmap());

#endif