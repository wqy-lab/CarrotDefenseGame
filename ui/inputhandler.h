#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <vector>
#include "../game/towers/tower.h"
#include "../game/cellentities.h"

class GameController;
class GameRenderer;
class TowerManager;
class PanelController;
class SpatialGrid;
class TutorialController;

class InputHandler {
public:
    InputHandler();

    void setGameController(GameController* gc) { m_gameController = gc; }
    void setGameRenderer(GameRenderer* gr) { m_gameRenderer = gr; }
    void setTowerManager(TowerManager* tm) { m_towerManager = tm; }
    void setPanelController(PanelController* pc) { m_panelController = pc; }
    void setSpatialGrid(SpatialGrid* sg) { m_spatialGrid = sg; }
    void setTutorialController(TutorialController* tc) { m_tutorialController = tc; }

    void handleMouseMove(QMouseEvent* event);
    void handleMousePress(QMouseEvent* event);
    void handleKeyPress(QKeyEvent* event);
    void handleResize(int width, int height);

private:
    GameController* m_gameController = nullptr;
    GameRenderer* m_gameRenderer = nullptr;
    TowerManager* m_towerManager = nullptr;
    PanelController* m_panelController = nullptr;
    SpatialGrid* m_spatialGrid = nullptr;
    TutorialController* m_tutorialController = nullptr;
};

#endif