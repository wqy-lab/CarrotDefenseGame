#include "towerfactory.h"
#include "arrowtower.h"
#include "canntower.h"
#include "icetower.h"
#include "poisontower.h"
#include "lighttower.h"
#include "suntower.h"

std::unique_ptr<Tower> createTower(TowerType type, int gridX, int gridY,
                                    double cellSize, double offsetX, double offsetY)
{
    switch (type) {
    case TowerType::Arrow:   return std::make_unique<ArrowTower>(gridX, gridY, cellSize, offsetX, offsetY);
    case TowerType::Cannon:  return std::make_unique<CannonTower>(gridX, gridY, cellSize, offsetX, offsetY);
    case TowerType::Ice:     return std::make_unique<IceTower>(gridX, gridY, cellSize, offsetX, offsetY);
    case TowerType::Poison:  return std::make_unique<PoisonTower>(gridX, gridY, cellSize, offsetX, offsetY);
    case TowerType::Lightning: return std::make_unique<LightningTower>(gridX, gridY, cellSize, offsetX, offsetY);
    case TowerType::Sun:     return std::make_unique<SunTower>(gridX, gridY, cellSize, offsetX, offsetY);
    }
    return std::make_unique<ArrowTower>(gridX, gridY, cellSize, offsetX, offsetY);
}