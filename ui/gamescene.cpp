#include "gamescene.h"
#include "gamerenderer.h"
#include "inputhandler.h"
#include "panelcontroller.h"
#include "gameoverlay.h"
#include "gamehud.h"
#include "tutorialcontroller.h"
#include "../game/gamecontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "towerpanel.h"
#include "towerselectionpopup.h"
#include "../game/config/datamanager.h"

GameScene::GameScene(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(800, 600);
    setFocusPolicy(Qt::StrongFocus);

    // Create subsystems
    m_spatialGrid = new SpatialGrid();
    m_gameController = new GameController(m_spatialGrid, this);
    m_towerManager = new TowerManager(m_spatialGrid, m_gameController, this);
    m_gameRenderer = new GameRenderer(this);
    m_inputHandler = new InputHandler();
    m_towerPanel = new TowerPanel(this);
    m_selectionPopup = new TowerSelectionPopup(this);
    m_panelController = new PanelController(this);

    // Connect subsystems
    m_gameRenderer->setGameController(m_gameController);
    m_gameRenderer->setSpatialGrid(m_spatialGrid);
    m_gameRenderer->setTowerManager(m_towerManager);
    m_gameRenderer->setTowerPanel(m_towerPanel);

    m_inputHandler->setGameController(m_gameController);
    m_inputHandler->setGameRenderer(m_gameRenderer);
    m_inputHandler->setTowerManager(m_towerManager);
    m_inputHandler->setPanelController(m_panelController);
    m_inputHandler->setSpatialGrid(m_spatialGrid);

    m_panelController->setGameController(m_gameController);
    m_panelController->setTowerManager(m_towerManager);
    m_panelController->setSpatialGrid(m_spatialGrid);
    m_panelController->setTowerPanel(m_towerPanel);
    m_panelController->setSelectionPopup(m_selectionPopup);

    // Connect signals
    connect(m_gameController, &GameController::statsChanged, this, &GameScene::statsChanged);
    connect(m_gameController, &GameController::gameEnded, this, &GameScene::gameEnded);
    connect(m_towerManager, &TowerManager::towersChanged, m_gameRenderer, QOverload<>::of(&QWidget::update));

    // Initialize map
    m_spatialGrid->initMap(DataManager::instance().mapData());

    // Set initial cell size and offset based on actual widget size
    int w = qMax(width(), 800);
    int h = qMax(height(), 600);
    double cellSize = qMin(w / m_spatialGrid->gridCols(), h / m_spatialGrid->gridRows());
    if (cellSize < 20) cellSize = 20;
    double offsetX = (w - m_spatialGrid->gridCols() * cellSize) / 2.0;
    double offsetY = (h - m_spatialGrid->gridRows() * cellSize) / 2.0;
    m_spatialGrid->setCellSize(cellSize);
    m_spatialGrid->setOffset(offsetX, offsetY);

    // Set renderer geometry AFTER spatial grid is properly configured
    m_gameRenderer->setGeometry(0, 0, w, h);

    // Game timer
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameScene::gameLoop);

    // Panel connections
    connect(m_towerPanel, &TowerPanel::upgradeClicked, m_panelController, &PanelController::onUpgradeClicked);
    connect(m_towerPanel, &TowerPanel::sellClicked, m_panelController, &PanelController::onSellClicked);
    connect(m_panelController, &PanelController::hideTowerPanelRequested, this, &GameScene::onPanelControllerHideTowerPanel);
    connect(m_gameController, &GameController::statsChanged, m_panelController, &PanelController::onStatsChanged);

    connect(m_panelController, &PanelController::towerSelectionShown,
            m_gameRenderer, &GameRenderer::setSelectedCell);
    connect(m_panelController, &PanelController::towerSelectionHidden,
            m_gameRenderer, &GameRenderer::clearSelectedCell);
    connect(m_panelController, &PanelController::towerPanelShown,
            m_gameRenderer, &GameRenderer::showRangeCircle);
    connect(m_panelController, &PanelController::towerPanelHidden,
            m_gameRenderer, &GameRenderer::hideAllPreviews);

    // Selection popup connections
    connect(m_selectionPopup, &TowerSelectionPopup::towerSelected, m_panelController, &PanelController::onTowerSelectedFromPopup);
    connect(m_selectionPopup, &TowerSelectionPopup::cancelled,
            m_panelController, &PanelController::onSelectionPopupCancelled);

    connect(m_towerPanel, &TowerPanel::panelHidden,
            m_panelController, &PanelController::onTowerPanelHidden);
    connect(m_towerPanel, &TowerPanel::panelHidden,
            m_gameRenderer, &GameRenderer::hideAllPreviews);

    connect(m_selectionPopup, &TowerSelectionPopup::towerButtonHovered,
            this, [this](TowerType type, bool hovered, int gx, int gy) {
        if (hovered) {
            double cs = m_spatialGrid->cellSize();
            m_gameRenderer->showTowerPreview(type, 1, gx, gy);
            double range = DataManager::instance().getTowerStats(type, 1).range * cs;
            m_gameRenderer->showRangeCircle(gx, gy, range);
        } else {
            m_gameRenderer->hideAllPreviews();
        }
    });

    connect(m_towerPanel, &TowerPanel::upgradeHovered,
            this, [this](bool hovered, int gx, int gy, TowerType type, int curLvl, int nextLvl) {
        double cs = m_spatialGrid->cellSize();
        if (hovered) {
            m_gameRenderer->showTowerPreview(type, nextLvl, gx, gy);
            double range = DataManager::instance().getTowerStats(type, nextLvl).range * cs;
            m_gameRenderer->showRangeCircle(gx, gy, range);
        } else {
            m_gameRenderer->hideAllPreviews();
            double range = DataManager::instance().getTowerStats(type, curLvl).range * cs;
            m_gameRenderer->showRangeCircle(gx, gy, range);
        }
    });

    connect(m_towerPanel, &TowerPanel::sellHovered,
            this, [this](bool hovered, int gx, int gy) {
        if (hovered) {
            m_gameRenderer->showSellHighlight(gx, gy);
        } else {
            m_gameRenderer->showSellHighlight(-1, -1);
        }
    });

    m_towerPanel->hide();
    m_selectionPopup->hide();

    m_overlay = new GameOverlay(this);
    m_overlay->setGeometry(rect());

    connect(m_overlay, &GameOverlay::continueClicked,
            this, &GameScene::onOverlayContinue);
    connect(m_overlay, &GameOverlay::exitToLevelSelectConfirmed,
            this, &GameScene::onOverlayExitConfirmed);

    // HUD
    m_gameHUD = new GameHUD(this);
    m_gameHUD->setGeometry(0, 0, w, m_gameHUD->sizeHint().height());
    m_gameHUD->raise();

    connect(m_gameHUD, &GameHUD::pauseClicked, this, [this]() {
        if (m_gameController->isPaused())
            hidePauseOverlay();
        else
            showPauseOverlay();
    });
    connect(m_gameController, &GameController::statsChanged,
            this, &GameScene::onStatsChanged);
}

void GameScene::startGame()
{
    m_gameController->startGame();

    // Tutorial level
    if (m_gameController->levelId() == 0) {
        auto* tutorial = new TutorialController(
            this, m_spatialGrid, m_towerManager, m_gameController, this);
        m_gameController->setOnEnemyKilled([tutorial]() {
            tutorial->onEnemyKilled();
        });
        tutorial->start();
        return;
    }

    m_gameHUD->setPausedState(false);
    m_clock.start();
    m_gameTimer->start(16);
}

void GameScene::pauseGame()
{
    m_gameController->pauseGame();
    m_gameHUD->setPausedState(true);
}

void GameScene::resumeGame()
{
    m_gameController->resumeGame();
    m_gameHUD->setPausedState(false);
}

void GameScene::resumeClock()
{
    m_clock.start();
    m_gameTimer->start(16);
}

void GameScene::resetGame()
{
    m_gameTimer->stop();
    m_overlay->hideOverlay();
    m_gameController->resetGame();
    m_towerManager->towers().clear();
    m_panelController->hideTowerPanel();
    m_panelController->clearSuppressFlag();
    update();
}

bool GameScene::isPaused() const
{
    return m_gameController->isPaused();
}

bool GameScene::isRunning() const
{
    return m_gameController->isRunning();
}

void GameScene::onStatsChanged()
{
    m_gameHUD->setGold(m_gameController->gold());
    m_gameHUD->setLives(m_gameController->lives());
    m_gameHUD->setWave(m_gameController->currentWave(),
                       m_gameController->totalWaves());
    m_gameHUD->setEnemies(m_gameController->enemiesInWave());
}

void GameScene::selectTowerType(TowerType type)
{
    m_towerManager->selectTowerType(type);
}

int GameScene::gold() const
{
    return m_gameController->gold();
}

int GameScene::lives() const
{
    return m_gameController->lives();
}

int GameScene::currentWave() const
{
    return m_gameController->currentWave();
}

int GameScene::totalWaves() const
{
    return m_gameController->totalWaves();
}

int GameScene::enemiesInWave() const
{
    return m_gameController->enemiesInWave();
}

bool GameScene::gameOver() const
{
    return m_gameController->isGameOver();
}

bool GameScene::victory() const
{
    return m_gameController->isVictory();
}

int GameScene::levelId() const
{
    return m_gameController->levelId();
}

void GameScene::setLevelId(int id)
{
    m_gameController->setLevelId(id);
}

void GameScene::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    m_gameRenderer->render();
}

void GameScene::mouseMoveEvent(QMouseEvent* event)
{
    m_inputHandler->handleMouseMove(event);
    QWidget::mouseMoveEvent(event);
}

void GameScene::mousePressEvent(QMouseEvent* event)
{
    m_inputHandler->handleMousePress(event);
    QWidget::mousePressEvent(event);
}

void GameScene::keyPressEvent(QKeyEvent* event)
{
    m_inputHandler->handleKeyPress(event);
    QWidget::keyPressEvent(event);
}

void GameScene::onPanelControllerHideTowerPanel()
{
    m_towerManager->setSelectedTowerPtr(nullptr);
    update();
}

void GameScene::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_inputHandler->handleResize(width(), height());
    m_gameRenderer->setGeometry(rect());
    m_overlay->setGeometry(rect());
    m_gameHUD->setGeometry(0, 0, width(), m_gameHUD->sizeHint().height());
}

void GameScene::gameLoop()
{
    if (!m_gameController->isRunning() || m_gameController->isPaused() || m_gameController->isGameOver()) {
        return;
    }

    double dt = m_clock.restart() / 1000.0;
    if (dt > 0.1) dt = 0.1;  // clamp to prevent spiral of death
    m_gameController->update(dt, m_towerManager->towers());
    m_gameRenderer->update();
}

void GameScene::showPauseOverlay()
{
    pauseGame();
    m_overlay->showPauseMenu();
    emit overlayVisibilityChanged(true);
}

void GameScene::hidePauseOverlay()
{
    m_overlay->hideOverlay();
    m_gameHUD->setPausedState(false);
    emit overlayVisibilityChanged(false);
}

void GameScene::onOverlayContinue()
{
    hidePauseOverlay();
    resumeGame();
}

void GameScene::onOverlayExitConfirmed()
{
    hidePauseOverlay();
    emit exitToLevelSelectRequested();
}