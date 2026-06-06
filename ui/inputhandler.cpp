#include "inputhandler.h"
#include "../game/gamecontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "../game/towers/tower.h"
#include "gamerenderer.h"
#include "panelcontroller.h"
#include "towerselectionpopup.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>

InputHandler::InputHandler()
{
}

void InputHandler::handleMouseMove(QMouseEvent* event)
{
    if (!m_spatialGrid) return;

    QPointF g = m_spatialGrid->pixelToGrid(event->pos());
    int gx = static_cast<int>(std::floor(g.x()));
    int gy = static_cast<int>(std::floor(g.y()));
    if (m_towerManager) {
        m_towerManager->setHoverGrid(gx, gy);

        if (m_towerManager->isPlacingTower() &&
            m_spatialGrid->isValidGridPos(gx, gy) &&
            !m_spatialGrid->isPathCell(gx, gy) &&
            !m_spatialGrid->isObstacleCell(gx, gy)) {
            m_towerManager->setShowRange(true);
        } else {
            m_towerManager->setShowRange(false);
        }
    }

    if (m_gameRenderer) {
        m_gameRenderer->update();
    }
}

void InputHandler::handleMousePress(QMouseEvent* event)
{
    if (!m_gameController || !m_gameController->isRunning() ||
        m_gameController->isPaused() || m_gameController->isGameOver()) return;
    if (event->button() != Qt::LeftButton) return;
    if (!m_spatialGrid) return;

    if (m_panelController && m_panelController->isSelectionPopupVisible()) {
        m_panelController->hideTowerSelectionPopup();
        return;
    }

    if (m_panelController && m_panelController->isTowerPanelVisible()) {
        m_panelController->hideTowerPanel();
        return;
    }

    if (m_panelController && m_panelController->shouldSuppressClick()) {
        return;
    }

    QPointF g = m_spatialGrid->pixelToGrid(event->pos());
    int gx = static_cast<int>(std::floor(g.x()));
    int gy = static_cast<int>(std::floor(g.y()));
    if (!m_spatialGrid->isValidGridPos(gx, gy)) return;

    if (m_towerManager) {
        Tower* t = m_towerManager->getTowerAt(gx, gy);
        if (t) {
            m_towerManager->setSelectedTowerPtr(t);
            if (m_panelController) {
                m_panelController->showTowerPanel(t);
            }
            return;
        }
    }

    auto cell = m_spatialGrid->getCellAt(gx, gy);
    if (!cell.enemies.empty()) {
        m_gameController->setPriorityEnemy(cell.enemies.front());
        if (m_panelController) {
            m_panelController->hideTowerPanel();
        }
        return;
    }
    if (!cell.obstacles.empty()) {
        m_gameController->setPriorityObstacle(cell.obstacles.front());
        if (m_panelController) {
            m_panelController->hideTowerPanel();
        }
        return;
    }

    // Click on empty ground - show tower selection popup
    if (m_towerManager &&
        !m_spatialGrid->isPathCell(gx, gy) &&
        !m_spatialGrid->isObstacleCell(gx, gy) &&
        !m_towerManager->getTowerAt(gx, gy)) {
        if (m_panelController) {
            m_panelController->showTowerSelectionPopup(gx, gy, event->globalPosition().toPoint());
        }
        return;
    }

    m_gameController->clearPriorityTarget();
    if (m_panelController) {
        m_panelController->hideTowerPanel();
    }
}

void InputHandler::handleKeyPress(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        if (m_panelController) {
            m_panelController->hideTowerPanel();
        }
    }
}

void InputHandler::handleResize(int width, int height)
{
    if (!m_spatialGrid) return;

    double w = static_cast<double>(width);
    double h = static_cast<double>(height);

    double cellSize = qMin(w / m_spatialGrid->gridCols(), h / m_spatialGrid->gridRows());
    if (cellSize < 20) cellSize = 20;
    double offsetX = (w - m_spatialGrid->gridCols() * cellSize) / 2.0;
    double offsetY = (h - m_spatialGrid->gridRows() * cellSize) / 2.0;
    m_spatialGrid->setCellSize(cellSize);
    m_spatialGrid->setOffset(offsetX, offsetY);

    if (m_gameRenderer) {
        m_gameRenderer->update();
    }
}