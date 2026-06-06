#include "bulletfactory.h"
#include "bullet.h"
#include "arrowbullet.h"
#include "cannonbullet.h"
#include "icebullet.h"
#include "poisonbullet.h"
#include "lightningbullet.h"
#include "../markers/marker.h"

std::unique_ptr<Bullet> createBullet(TowerType type, const QPointF& start,
                                      const QPointF& direction, double damage,
                                      double splashRadius,
                                      const QColor& color,
                                      std::vector<std::unique_ptr<Marker>> markers,
                                      int penetration)
{
    switch (type) {
    case TowerType::Arrow: {
        auto b = std::make_unique<ArrowBullet>(start, direction, damage, color);
        b->setMarkers(std::move(markers));
        b->setPenetration(penetration);
        return b;
    }
    case TowerType::Cannon: {
        auto b = std::make_unique<CannonBullet>(start, direction, damage, splashRadius, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case TowerType::Ice: {
        auto b = std::make_unique<IceBullet>(start, direction, damage, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case TowerType::Poison: {
        auto b = std::make_unique<PoisonBullet>(start, direction, damage, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    case TowerType::Lightning: {
        auto b = std::make_unique<LightningBullet>(start, direction, damage, 3, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    default: {
        auto b = std::make_unique<ArrowBullet>(start, direction, damage, color);
        b->setMarkers(std::move(markers));
        return b;
    }
    }
}