#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "../game/config/datamanager.h"

#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stacked(nullptr)
    , m_menu(nullptr)
    , m_levelSelect(nullptr)
    , m_scene(nullptr)
{
    ui->setupUi(this);

    m_stacked = qobject_cast<QStackedWidget*>(ui->centralwidget);

    // Create pages
    m_menu = new MainMenuWidget(this);
    m_levelSelect = new LevelSelectWidget(this);
    m_scene = new GameScene(this);

    m_stacked->addWidget(m_menu);       // page 0
    m_stacked->addWidget(m_levelSelect); // page 1
    m_stacked->addWidget(m_scene);       // page 2

    // Start on main menu
    showMenuPage();

    // --- MainMenu signals ---
    connect(m_menu, &MainMenuWidget::startGameClicked,
            this, &MainWindow::onStartGame);
    connect(m_menu, &MainMenuWidget::levelSelectClicked,
            this, [this]() { m_levelSelect->clearSelection(); m_stacked->setCurrentIndex(1); });

    // --- LevelSelect signals ---
    connect(m_levelSelect, &LevelSelectWidget::levelSelected,
            this, &MainWindow::onLevelSelected);
    connect(m_levelSelect, &LevelSelectWidget::backClicked,
            this, [this]() { m_stacked->setCurrentIndex(0); });

    // --- GameScene signals ---
    connect(m_scene, &GameScene::statsChanged,
            this, &MainWindow::onStatsChanged);
    connect(m_scene, &GameScene::gameEnded,
            this, &MainWindow::onGameEnded);
    connect(m_scene, &GameScene::exitToLevelSelectRequested,
            this, &MainWindow::onExitToLevelSelect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ========== Page Navigation ==========

void MainWindow::showMenuPage()
{
    m_stacked->setCurrentIndex(0);
}

void MainWindow::showGamePage()
{
    m_stacked->setCurrentIndex(2);
}

// ========== Level Loading ==========

void MainWindow::loadAndStartLevel(int id, const QString& file)
{
    if (!DataManager::instance().loadLevel(file))
    {
        qWarning() << "Failed to load level:" << file;
        return;
    }

    m_currentLevelId = id;
    m_currentLevelFile = file;
    m_scene->setLevelId(id);
    m_scene->resetGame();
    m_scene->startGame();

    showGamePage();
}

// ========== Main Menu Handlers ==========

void MainWindow::onStartGame()
{
    const auto& levels = DataManager::instance().levels();
    if (!levels.empty())
        loadAndStartLevel(levels[0].id, levels[0].file);
}

void MainWindow::onLevelSelected(int id, const QString& file)
{
    loadAndStartLevel(id, file);
}

// ========== Game Over & Overlay ==========

void MainWindow::onGameEnded(bool won, int levelId)
{
    if (won)
    {
#ifndef QT_DEBUG
        const auto& levels = DataManager::instance().levels();
        for (size_t i = 0; i < levels.size(); ++i)
        {
            if (levels[i].id == levelId && i + 1 < levels.size())
            {
                m_levelSelect->unlockLevel(levels[i + 1].id);
                break;
            }
        }
#endif
    }

    cleanupOverlay();
    m_resultOverlay = new GameResultWidget(won, m_currentLevelId, this);
    m_resultOverlay->setGeometry(m_stacked->geometry());
    m_resultOverlay->raise();
    m_resultOverlay->show();

    connect(m_resultOverlay, &GameResultWidget::retryClicked,
            this, &MainWindow::onRetry);
    connect(m_resultOverlay, &GameResultWidget::nextLevelClicked,
            this, &MainWindow::onNextLevel);
    connect(m_resultOverlay, &GameResultWidget::menuClicked,
            this, &MainWindow::onMenu);
}

void MainWindow::cleanupOverlay()
{
    if (m_resultOverlay)
    {
        m_resultOverlay->deleteLater();
        m_resultOverlay = nullptr;
    }
}

void MainWindow::onRetry()
{
    cleanupOverlay();
    loadAndStartLevel(m_currentLevelId, m_currentLevelFile);
}

void MainWindow::onNextLevel()
{
    cleanupOverlay();

    const auto& levels = DataManager::instance().levels();
    for (size_t i = 0; i < levels.size(); ++i)
    {
        if (levels[i].id == m_currentLevelId && i + 1 < levels.size())
        {
            loadAndStartLevel(levels[i + 1].id, levels[i + 1].file);
            return;
        }
    }
}

void MainWindow::onMenu()
{
    cleanupOverlay();
    showMenuPage();
}

void MainWindow::onExitToLevelSelect()
{
    m_scene->resetGame();
    m_stacked->setCurrentIndex(1);
}

void MainWindow::onStatsChanged()
{
}
