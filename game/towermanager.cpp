#include "towermanager.h"
#include "spatialgrid.h"
#include "gamecontroller.h"
#include "towers/towerfactory.h"
#include "towers/tower.h"
#include <algorithm>

TowerManager::TowerManager(SpatialGrid* spatialGrid, GameController* gameController, QObject* parent)
    : QObject(parent)
    , m_spatialGrid(spatialGrid)
    , m_gameController(gameController)
{
}

void TowerManager::selectTowerType(TowerType type)
{
    // Tower placement now via popup, not toolbar
    // This method is kept for potential hotkey shortcuts but does nothing
    Q_UNUSED(type);
    m_placingTower = false;
    m_showRange = false;
}

bool TowerManager::canPlaceTower(int gx, int gy) const
{
    if (!m_spatialGrid->isValidGridPos(gx, gy)) return false;
    if (m_spatialGrid->isPathCell(gx, gy)) return false;
    if (m_spatialGrid->isObstacleCell(gx, gy)) return false;
    if (getTowerAt(gx, gy) != nullptr) return false;
    return true;
}

void TowerManager::placeTower(int gx, int gy)
{
    if (!canPlaceTower(gx, gy)) return;

    auto t = createTower(m_selectedTower, gx, gy,
                        m_spatialGrid->cellSize(),
                        m_spatialGrid->offsetX(),
                        m_spatialGrid->offsetY());
    if (m_gameController->gold() < t->cost()) return;

    m_gameController->spendGold(t->cost());
    m_towers.push_back(std::move(t));
    emit towersChanged();
}

void TowerManager::placeTowerAt(int gx, int gy, TowerType type)
{
    if (!canPlaceTower(gx, gy)) return;

    auto t = createTower(type, gx, gy,
                        m_spatialGrid->cellSize(),
                        m_spatialGrid->offsetX(),
                        m_spatialGrid->offsetY());
    if (m_gameController->gold() < t->cost()) return;

    m_gameController->spendGold(t->cost());
    m_towers.push_back(std::move(t));
    emit towersChanged();
}

void TowerManager::removeTower(Tower* tower)
{
    auto it = std::find_if(m_towers.begin(), m_towers.end(),
        [tower](auto& ptr){ return ptr.get() == tower; });
    if (it != m_towers.end()) {
        m_towers.erase(it);
        emit towersChanged();
    }
}

void TowerManager::addTower(std::unique_ptr<Tower> tower)
{
    m_towers.push_back(std::move(tower));
    emit towersChanged();
}

Tower* TowerManager::getTowerAt(int gx, int gy) const
{
    for (auto& t : m_towers) {
        if (t->gridX() == gx && t->gridY() == gy) {
            return t.get();
        }
    }
    return nullptr;
}