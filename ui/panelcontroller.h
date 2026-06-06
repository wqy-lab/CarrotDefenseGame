#ifndef PANELCONTROLLER_H
#define PANELCONTROLLER_H

#include <QObject>
#include <QWidget>
#include "../game/towers/tower.h"

class Tower;
class TowerPanel;
class TowerSelectionPopup;
class GameController;
class TowerManager;
class SpatialGrid;

class PanelController : public QObject {
    Q_OBJECT

public:
    explicit PanelController(QWidget* gameWidget, QObject* parent = nullptr);
    ~PanelController() = default;

    void setGameController(GameController* gc) { m_gameController = gc; }
    void setTowerManager(TowerManager* tm) { m_towerManager = tm; }
    void setSpatialGrid(SpatialGrid* sg) { m_spatialGrid = sg; }
    void setTowerPanel(TowerPanel* tp) { m_towerPanel = tp; }
    void setSelectionPopup(TowerSelectionPopup* popup) { m_selectionPopup = popup; }

public slots:
    void showTowerPanel(Tower* tower);
    void hideTowerPanel();
    void onUpgradeClicked();
    void onSellClicked();
    void showTowerSelectionPopup(int gridX, int gridY, const QPoint& globalPos);
    void hideTowerSelectionPopup();
    void onTowerSelectedFromPopup(TowerType type);
    void onStatsChanged();
    void onTowerPanelHidden();

    bool shouldSuppressClick();

private slots:

signals:
    void hideTowerPanelRequested();
    void towerSelectionShown(int gridX, int gridY);
    void towerSelectionHidden();

private:
    QWidget* m_gameWidget = nullptr;
    TowerPanel* m_towerPanel = nullptr;
    TowerSelectionPopup* m_selectionPopup = nullptr;
    GameController* m_gameController = nullptr;
    TowerManager* m_towerManager = nullptr;
    SpatialGrid* m_spatialGrid = nullptr;
    Tower* m_currentTower = nullptr;
    int m_pendingGridX = -1;
    int m_pendingGridY = -1;
    bool m_suppressNextClick = false;
};

#endif