#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "../game/config/datamanager.h"

#include <QPushButton>
#include <QLabel>
#include <QToolBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stacked(nullptr)
    , m_menu(nullptr)
    , m_levelSelect(nullptr)
    , m_scene(nullptr)
    , m_btnStart(nullptr)
    , m_btnPause(nullptr)
    , m_btnArrow(nullptr)
    , m_btnCannon(nullptr)
    , m_btnIce(nullptr)
    , m_btnPoison(nullptr)
    , m_btnLightning(nullptr)
    , m_btnSun(nullptr)
    , m_lblGold(nullptr)
    , m_lblLives(nullptr)
    , m_lblWave(nullptr)
    , m_lblInfo(nullptr)
    , m_selectedType(TowerType::Arrow)
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

    // Setup toolbar + status bar (for game page)
    setupToolbar();
    setupStatusBar();

    // Start on main menu
    showMenuPage();

    // --- MainMenu signals ---
    connect(m_menu, &MainMenuWidget::startGameClicked,
            this, &MainWindow::onStartGame);
    connect(m_menu, &MainMenuWidget::levelSelectClicked,
            this, [this]() { m_stacked->setCurrentIndex(1); });

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ========== Page Navigation ==========

void MainWindow::showMenuPage()
{
    ui->toolBar->hide();
    ui->statusbar->hide();
    m_stacked->setCurrentIndex(0);
}

void MainWindow::showGamePage()
{
    ui->toolBar->show();
    ui->statusbar->show();
    m_stacked->setCurrentIndex(2);
    updateStatusBar();
    updateTowerButtons();
}

// ========== Level Loading ==========

void MainWindow::loadAndStartLevel(int id, const QString& file)
{
    if (!DataManager::instance().loadLevel(file))
    {
        m_lblInfo->setText("Failed to load level!");
        return;
    }

    m_currentLevelId = id;
    m_currentLevelFile = file;
    m_scene->setLevelId(id);
    m_scene->resetGame();
    m_scene->startGame();

    showGamePage();
    m_btnStart->setEnabled(false);
    m_btnPause->setEnabled(true);
    m_btnPause->setText("Pause");
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
    Q_UNUSED(levelId);
    m_btnStart->setEnabled(true);
    m_btnPause->setEnabled(false);

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
    int nextId = m_currentLevelId + 1;
    QString nextFile = QString("config/levels/level%1.json").arg(nextId);
    loadAndStartLevel(nextId, nextFile);
}

void MainWindow::onMenu()
{
    cleanupOverlay();
    showMenuPage();
}

// ========== Toolbar ==========

void MainWindow::setupToolbar()
{
    QToolBar* toolbar = ui->toolBar;

    m_btnStart = new QPushButton("Start Wave");
    m_btnStart->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 6px 16px; "
        "border: none; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #666; }");
    toolbar->addWidget(m_btnStart);

    m_btnPause = new QPushButton("Pause");
    m_btnPause->setEnabled(false);
    toolbar->addWidget(m_btnPause);

    toolbar->addSeparator();

    QLabel* towerLabel = new QLabel(" Towers: ");
    toolbar->addWidget(towerLabel);

    m_btnArrow = new QPushButton(QString("Arrow %1g").arg(
        DataManager::instance().getTowerStats(TowerType::Arrow).cost));
    m_btnArrow->setCheckable(true);
    m_btnArrow->setChecked(true);
    m_btnArrow->setStyleSheet(
        "QPushButton { background-color: #8BC34A; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnArrow);

    m_btnCannon = new QPushButton(QString("Cannon %1g").arg(
        DataManager::instance().getTowerStats(TowerType::Cannon).cost));
    m_btnCannon->setCheckable(true);
    m_btnCannon->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnCannon);

    m_btnIce = new QPushButton(QString("Ice %1g").arg(
        DataManager::instance().getTowerStats(TowerType::Ice).cost));
    m_btnIce->setCheckable(true);
    m_btnIce->setStyleSheet(
        "QPushButton { background-color: #64B5F6; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnIce);

    m_btnPoison = new QPushButton(QString("Poison %1g").arg(
        DataManager::instance().getTowerStats(TowerType::Poison).cost));
    m_btnPoison->setCheckable(true);
    m_btnPoison->setStyleSheet(
        "QPushButton { background-color: #78C850; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnPoison);

    m_btnLightning = new QPushButton(QString("Lightning %1g").arg(
        DataManager::instance().getTowerStats(TowerType::Lightning).cost));
    m_btnLightning->setCheckable(true);
    m_btnLightning->setStyleSheet(
        "QPushButton { background-color: #DCC830; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnLightning);

    m_btnSun = new QPushButton(QString("Sun %1g").arg(
        DataManager::instance().getTowerStats(TowerType::Sun).cost));
    m_btnSun->setCheckable(true);
    m_btnSun->setStyleSheet(
        "QPushButton { background-color: #FFC107; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnSun);

    toolbar->addSeparator();

    QLabel* helpLabel = new QLabel(" Click green cells to place towers. Cannot place on road. ");
    helpLabel->setStyleSheet("color: #AAA;");
    toolbar->addWidget(helpLabel);

    // Toolbar signal connections
    connect(m_btnStart, &QPushButton::clicked, this, &MainWindow::onStartWave);
    connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::onPauseResume);
    connect(m_btnArrow, &QPushButton::clicked, this, &MainWindow::onSelectArrow);
    connect(m_btnCannon, &QPushButton::clicked, this, &MainWindow::onSelectCannon);
    connect(m_btnIce, &QPushButton::clicked, this, &MainWindow::onSelectIce);
    connect(m_btnPoison, &QPushButton::clicked, this, &MainWindow::onSelectPoison);
    connect(m_btnLightning, &QPushButton::clicked, this, &MainWindow::onSelectLightning);
    connect(m_btnSun, &QPushButton::clicked, this, &MainWindow::onSelectSun);
}

// ========== Status Bar ==========

void MainWindow::setupStatusBar()
{
    m_lblGold = new QLabel();
    m_lblLives = new QLabel();
    m_lblWave = new QLabel();
    m_lblInfo = new QLabel();

    statusBar()->addPermanentWidget(m_lblGold);
    statusBar()->addPermanentWidget(m_lblLives);
    statusBar()->addPermanentWidget(m_lblWave);
    statusBar()->addWidget(m_lblInfo, 1);
}

void MainWindow::updateStatusBar()
{
    m_lblGold->setText(QString("<b>Gold:</b> %1").arg(m_scene->gold()));
    m_lblLives->setText(QString("<b>Lives:</b> %1").arg(m_scene->lives()));
    m_lblWave->setText(QString("<b>Wave:</b> %1/%2")
        .arg(m_scene->currentWave())
        .arg(m_scene->totalWaves()));

    if (m_scene->isRunning()) {
        m_lblInfo->setText(QString("Enemies: %1  |  Click grid to place towers")
            .arg(m_scene->enemiesInWave()));
    }
}

void MainWindow::updateTowerButtons()
{
    int gold = m_scene->gold();
    m_btnArrow->setEnabled(gold >=
        DataManager::instance().getTowerStats(TowerType::Arrow).cost);
    m_btnCannon->setEnabled(gold >=
        DataManager::instance().getTowerStats(TowerType::Cannon).cost);
    m_btnIce->setEnabled(gold >=
        DataManager::instance().getTowerStats(TowerType::Ice).cost);
    m_btnPoison->setEnabled(gold >=
        DataManager::instance().getTowerStats(TowerType::Poison).cost);
    m_btnLightning->setEnabled(gold >=
        DataManager::instance().getTowerStats(TowerType::Lightning).cost);
    m_btnSun->setEnabled(gold >=
        DataManager::instance().getTowerStats(TowerType::Sun).cost);
}

// ========== Game Control Slots ==========

void MainWindow::onStartWave()
{
    m_scene->startGame();
    m_btnStart->setEnabled(false);
    m_btnPause->setEnabled(true);
    m_btnPause->setText("Pause");
    updateStatusBar();
}

void MainWindow::onPauseResume()
{
    if (m_scene->isPaused()) {
        m_scene->resumeGame();
        m_btnPause->setText("Pause");
    } else {
        m_scene->pauseGame();
        m_btnPause->setText("Resume");
    }
}

void MainWindow::onStatsChanged()
{
    updateStatusBar();
    updateTowerButtons();
}

// ========== Tower Select Slots ==========

void MainWindow::onSelectArrow()
{
    m_selectedType = TowerType::Arrow;
    m_btnArrow->setChecked(true); m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false); m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false); m_btnSun->setChecked(false);
    m_scene->selectTowerType(TowerType::Arrow);
}

void MainWindow::onSelectCannon()
{
    m_selectedType = TowerType::Cannon;
    m_btnArrow->setChecked(false); m_btnCannon->setChecked(true);
    m_btnIce->setChecked(false); m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false); m_btnSun->setChecked(false);
    m_scene->selectTowerType(TowerType::Cannon);
}

void MainWindow::onSelectIce()
{
    m_selectedType = TowerType::Ice;
    m_btnArrow->setChecked(false); m_btnCannon->setChecked(false);
    m_btnIce->setChecked(true); m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false); m_btnSun->setChecked(false);
    m_scene->selectTowerType(TowerType::Ice);
}

void MainWindow::onSelectPoison()
{
    m_selectedType = TowerType::Poison;
    m_btnArrow->setChecked(false); m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false); m_btnPoison->setChecked(true);
    m_btnLightning->setChecked(false); m_btnSun->setChecked(false);
    m_scene->selectTowerType(TowerType::Poison);
}

void MainWindow::onSelectLightning()
{
    m_selectedType = TowerType::Lightning;
    m_btnArrow->setChecked(false); m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false); m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(true); m_btnSun->setChecked(false);
    m_scene->selectTowerType(TowerType::Lightning);
}

void MainWindow::onSelectSun()
{
    m_selectedType = TowerType::Sun;
    m_btnArrow->setChecked(false); m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false); m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false); m_btnSun->setChecked(true);
    m_scene->selectTowerType(TowerType::Sun);
}
