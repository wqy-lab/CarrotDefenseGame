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
    void onSelectionPopupCancelled();
    void onStatsChanged();
    void onTowerPanelHidden();

    bool shouldSuppressClick();
    void clearSuppressFlag();
    bool isSelectionPopupVisible() const { return m_selectionPopupVisible; }
    bool isTowerPanelVisible() const { return m_towerPanelVisible; }

private slots:

signals:
    void hideTowerPanelRequested();
    void towerSelectionShown(int gridX, int gridY);
    void towerSelectionHidden();
    void towerPanelShown(int gx, int gy, double rangePx);
    void towerPanelHidden();

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
    bool m_towerPanelVisible = false;
    bool m_selectionPopupVisible = false;
};

#endif