#ifndef TOWERMANAGER_H
#define TOWERMANAGER_H

#include <QObject>
#include <vector>
#include <memory>
#include "towers/tower.h"

class Tower;
class GameController;
class SpatialGrid;

class TowerManager : public QObject {
    Q_OBJECT

public:
    explicit TowerManager(SpatialGrid* spatialGrid, GameController* gameController, QObject* parent = nullptr);
    ~TowerManager() = default;

    void selectTowerType(TowerType type);
    TowerType selectedTower() const { return m_selectedTower; }
    bool isPlacingTower() const { return m_placingTower; }

    int hoverGridX() const { return m_hoverGridX; }
    int hoverGridY() const { return m_hoverGridY; }
    void setHoverGrid(int x, int y) { m_hoverGridX = x; m_hoverGridY = y; }

    bool showRange() const { return m_showRange; }
    void setShowRange(bool show) { m_showRange = show; }

    Tower* selectedTowerPtr() const { return m_selectedTowerPtr; }
    void setSelectedTowerPtr(Tower* t) { m_selectedTowerPtr = t; }

    const std::vector<std::unique_ptr<Tower>>& towers() const { return m_towers; }
    std::vector<std::unique_ptr<Tower>>& towers() { return m_towers; }

    bool canPlaceTower(int gx, int gy) const;
    void placeTower(int gx, int gy);
    void placeTowerAt(int gx, int gy, TowerType type);
    void removeTower(Tower* tower);
    void addTower(std::unique_ptr<Tower> tower);

    Tower* getTowerAt(int gx, int gy) const;

signals:
    void towersChanged();

private:
    SpatialGrid* m_spatialGrid = nullptr;
    GameController* m_gameController = nullptr;

    std::vector<std::unique_ptr<Tower>> m_towers;
    TowerType m_selectedTower = TowerType::Arrow;
    bool m_placingTower = false;
    int m_hoverGridX = -1;
    int m_hoverGridY = -1;
    bool m_showRange = false;
    Tower* m_selectedTowerPtr = nullptr;
};

#endif