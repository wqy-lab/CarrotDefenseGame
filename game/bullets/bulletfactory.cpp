#include "bulletfactory.h"
#include "normalbullet.h"

std::unique_ptr<Bullet> createBullet(BulletType type, const QPointF& start,
                                      const QPointF& target, double damage,
                                      double splashRadius, double slowFactor,
                                      double slowDuration, double poisonDps,
                                      double poisonDuration, int chainCount,
                                      const QColor& color)
{
    switch (type) {
    case BulletType::Normal:
        return std::make_unique<NormalBullet>(start, target, damage, splashRadius,
                                             slowFactor, slowDuration, poisonDps,
                                             poisonDuration, chainCount, color);
    }
    return std::make_unique<NormalBullet>(start, target, damage, splashRadius,
                                         slowFactor, slowDuration, poisonDps,
                                         poisonDuration, chainCount, color);
}