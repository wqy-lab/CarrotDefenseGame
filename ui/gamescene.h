#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include "../game/towers/tower.h"

class GameController;
class GameRenderer;
class InputHandler;
class PanelController;
class TowerManager;
class SpatialGrid;
class TowerPanel;
class TowerSelectionPopup;
class GameOverlay;
class GameHUD;

class GameScene : public QWidget {
    Q_OBJECT

public:
    explicit GameScene(QWidget* parent = nullptr);
    ~GameScene() = default;

    void startGame();
    void pauseGame();
    void resumeGame();
    void resumeClock();
    void resetGame();
    bool isPaused() const;
    bool isRunning() const;

    void selectTowerType(TowerType type);
    void showPauseOverlay();
    void hidePauseOverlay();
    int gold() const;
    int lives() const;
    int currentWave() const;
    int totalWaves() const;
    int enemiesInWave() const;
    bool gameOver() const;
    bool victory() const;
    int levelId() const;
    void setLevelId(int id);

signals:
    void statsChanged();
    void gameEnded(bool won, int levelId);
    void exitToLevelSelectRequested();
    void overlayVisibilityChanged(bool visible);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void gameLoop();
    void onPanelControllerHideTowerPanel();
    void onOverlayContinue();
    void onOverlayExitConfirmed();
    void onStatsChanged();

private:
    SpatialGrid* m_spatialGrid = nullptr;
    GameController* m_gameController = nullptr;
    TowerManager* m_towerManager = nullptr;
    GameRenderer* m_gameRenderer = nullptr;
    InputHandler* m_inputHandler = nullptr;
    PanelController* m_panelController = nullptr;
    TowerPanel* m_towerPanel = nullptr;
    TowerSelectionPopup* m_selectionPopup = nullptr;
    GameOverlay* m_overlay = nullptr;
    GameHUD* m_gameHUD = nullptr;

    QTimer* m_gameTimer = nullptr;
    QElapsedTimer m_clock;
};

#endif