#include "bulletfactory.h"
#include "bullet.h"
#include "arrowbullet.h"
#include "cannonbullet.h"
#include "icebullet.h"
#include "poisonbullet.h"
#include "lightningbullet.h"
#include "../markers/marker.h"

std::unique_ptr<Bullet> createBullet(BulletType type, const QPointF& start,
                                      const QPointF& target, double damage,
                                      double slowFactor, double slowDuration,
                                      double poisonDps, double poisonDuration,
                                      double splashRadius, int chainCount,
                                      const QColor& color)
{
    switch (type) {
    case BulletType::Arrow:
        return std::make_unique<ArrowBullet>(start, target, damage,
                                             slowFactor, slowDuration,
                                             poisonDps, poisonDuration, color);
    case BulletType::Cannon:
        return std::make_unique<CannonBullet>(start, target, damage,
                                              splashRadius, color);
    case BulletType::Ice:
        return std::make_unique<IceBullet>(start, target, damage,
                                           slowFactor, slowDuration, color);
    case BulletType::Poison:
        return std::make_unique<PoisonBullet>(start, target, damage,
                                               poisonDps, poisonDuration, color);
    case BulletType::Lightning:
        return std::make_unique<LightningBullet>(start, target, damage,
                                                  chainCount, color);
    }
    return std::make_unique<ArrowBullet>(start, target, damage,
                                          slowFactor, slowDuration,
                                          poisonDps, poisonDuration, color);
}