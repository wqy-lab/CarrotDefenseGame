#include "panelcontroller.h"
#include "../game/gamecontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "../game/towers/tower.h"
#include "towerpanel.h"
#include "towerselectionpopup.h"

PanelController::PanelController(QWidget* gameWidget, QObject* parent)
    : QObject(parent)
    , m_gameWidget(gameWidget)
{
}

void PanelController::showTowerPanel(Tower* tower)
{
    if (!tower || !m_towerPanel || !m_spatialGrid) return;

    m_currentTower = tower;
    m_towerPanel->setTower(tower);

    QPointF towerPos = m_spatialGrid->gridToPixel(tower->gridX(), tower->gridY());
    const int panelWidth = 180;
    const int panelHeight = 140;
    int panelX = static_cast<int>(towerPos.x() - panelWidth / 2);
    int panelY = static_cast<int>(towerPos.y() - panelHeight - m_spatialGrid->cellSize() * 0.8);

    if (panelX < 0) panelX = 0;
    if (panelX + panelWidth > m_gameWidget->width()) panelX = m_gameWidget->width() - panelWidth;
    if (panelY < 0) panelY = static_cast<int>(towerPos.y() + m_spatialGrid->cellSize() * 0.8);

    QPoint globalPos = m_gameWidget->mapToGlobal(QPoint(panelX, panelY));
    m_towerPanel->move(globalPos);
    m_towerPanel->show();
    m_towerPanel->raise();
}

void PanelController::hideTowerPanel()
{
    if (m_towerPanel) {
        m_towerPanel->hide();
    }
    m_currentTower = nullptr;
    if (m_towerManager) {
        m_towerManager->setSelectedTowerPtr(nullptr);
    }
    emit hideTowerPanelRequested();
}

void PanelController::onUpgradeClicked()
{
    if (!m_currentTower || !m_gameController) return;

    int cost = m_currentTower->upgradeCost();
    if (m_gameController->gold() < cost) return;

    m_gameController->spendGold(cost);
    m_currentTower->upgrade();
    m_towerPanel->updateInfo();
    emit m_gameController->statsChanged();
}

void PanelController::onSellClicked()
{
    if (!m_currentTower || !m_gameController || !m_towerManager) return;

    m_gameController->addGold(m_currentTower->sellValue());
    m_towerManager->removeTower(m_currentTower);
    hideTowerPanel();
    emit m_gameController->statsChanged();
}

void PanelController::onStatsChanged()
{
    int gold = m_gameController->gold();
    if (m_selectionPopup && m_selectionPopup->isVisible()) {
        m_selectionPopup->updateGold(gold);
    }
    if (m_towerPanel && m_towerPanel->isVisible()) {
        m_towerPanel->updateGold(gold);
    }
}

void PanelController::showTowerSelectionPopup(int gridX, int gridY, const QPoint& globalPos)
{
    if (!m_selectionPopup || !m_gameController || !m_spatialGrid) return;

    m_pendingGridX = gridX;
    m_pendingGridY = gridY;

    m_selectionPopup->setGridPos(gridX, gridY);
    m_selectionPopup->updateGold(m_gameController->gold());

    // Position popup near click but offset so it doesn't cover the cell
    QPointF cellCenter = m_spatialGrid->gridToPixel(gridX, gridY);
    QPoint adjustedPos = globalPos;
    // Shift slightly to the right and down from click point
    adjustedPos += QPoint(10, 10);

    m_selectionPopup->move(adjustedPos);
    m_selectionPopup->show();
    m_selectionPopup->raise();
}

void PanelController::hideTowerSelectionPopup()
{
    if (m_selectionPopup) {
        m_selectionPopup->hide();
    }
    m_pendingGridX = -1;
    m_pendingGridY = -1;
}

void PanelController::onTowerSelectedFromPopup(TowerType type)
{
    if (!m_towerManager) return;

    int gx = m_pendingGridX;
    int gy = m_pendingGridY;

    hideTowerSelectionPopup();

    if (gx >= 0 && gy >= 0) {
        m_towerManager->placeTowerAt(gx, gy, type);
    }
}