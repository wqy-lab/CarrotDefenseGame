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
                                      double splashRadius,
                                      const QColor& color,
                                      std::vector<std::unique_ptr<Marker>> markers)
{
    switch (type) {
    case BulletType::Arrow: {
        auto b = std::make_unique<ArrowBullet>(start, target, damage, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case BulletType::Cannon: {
        auto b = std::make_unique<CannonBullet>(start, target, damage, splashRadius, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case BulletType::Ice: {
        auto b = std::make_unique<IceBullet>(start, target, damage, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case BulletType::Poison: {
        auto b = std::make_unique<PoisonBullet>(start, target, damage, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case BulletType::Lightning: {
        auto b = std::make_unique<LightningBullet>(start, target, damage, 3, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    }
    auto b = std::make_unique<ArrowBullet>(start, target, damage, color);
    b->setMarkers(std::move(markers));
    return b;
}