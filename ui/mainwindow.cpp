#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QLabel>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

    m_scene = new GameScene(this);
    setCentralWidget(m_scene);

    // 创建工具栏按钮
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

    m_btnArrow = new QPushButton("Arrow 40g");
    m_btnArrow->setCheckable(true);
    m_btnArrow->setChecked(true);
    m_btnArrow->setStyleSheet(
        "QPushButton { background-color: #8BC34A; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnArrow);

    m_btnCannon = new QPushButton("Cannon 80g");
    m_btnCannon->setCheckable(true);
    m_btnCannon->setStyleSheet(
        "QPushButton { background-color: #FF9800; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnCannon);

    m_btnIce = new QPushButton("Ice 60g");
    m_btnIce->setCheckable(true);
    m_btnIce->setStyleSheet(
        "QPushButton { background-color: #64B5F6; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnIce);

    m_btnPoison = new QPushButton("Poison 65g");
    m_btnPoison->setCheckable(true);
    m_btnPoison->setStyleSheet(
        "QPushButton { background-color: #78C850; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnPoison);

    m_btnLightning = new QPushButton("Lightning 90g");
    m_btnLightning->setCheckable(true);
    m_btnLightning->setStyleSheet(
        "QPushButton { background-color: #DCC830; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnLightning);

    m_btnSun = new QPushButton("Sun 50g");
    m_btnSun->setCheckable(true);
    m_btnSun->setStyleSheet(
        "QPushButton { background-color: #FFC107; color: #333; padding: 5px 10px; border-radius: 3px; }"
        "QPushButton:checked { border: 2px solid white; }");
    toolbar->addWidget(m_btnSun);

    toolbar->addSeparator();

    QLabel* helpLabel = new QLabel(" Click green cells to place towers. Cannot place on road. ");
    helpLabel->setStyleSheet("color: #AAA;");
    toolbar->addWidget(helpLabel);

    // 创建状态栏标签
    m_lblGold = new QLabel();
    m_lblLives = new QLabel();
    m_lblWave = new QLabel();
    m_lblInfo = new QLabel();

    statusBar()->addPermanentWidget(m_lblGold);
    statusBar()->addPermanentWidget(m_lblLives);
    statusBar()->addPermanentWidget(m_lblWave);
    statusBar()->addWidget(m_lblInfo, 1);

    // 信号槽连接
    connect(m_btnStart, &QPushButton::clicked, this, &MainWindow::onStartWave);
    connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::onPauseResume);
    connect(m_btnArrow, &QPushButton::clicked, this, &MainWindow::onSelectArrow);
    connect(m_btnCannon, &QPushButton::clicked, this, &MainWindow::onSelectCannon);
    connect(m_btnIce, &QPushButton::clicked, this, &MainWindow::onSelectIce);
    connect(m_btnPoison, &QPushButton::clicked, this, &MainWindow::onSelectPoison);
    connect(m_btnLightning, &QPushButton::clicked, this, &MainWindow::onSelectLightning);
    connect(m_btnSun, &QPushButton::clicked, this, &MainWindow::onSelectSun);
    connect(m_scene, &GameScene::statsChanged, this, &MainWindow::onStatsChanged);
    connect(m_scene, &GameScene::gameEnded, this, &MainWindow::onGameEnded);

    updateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

void MainWindow::onSelectArrow()
{
    m_selectedType = TowerType::Arrow;
    m_btnArrow->setChecked(true);
    m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false);
    m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false);
    m_scene->selectTowerType(TowerType::Arrow);
}

void MainWindow::onSelectCannon()
{
    m_selectedType = TowerType::Cannon;
    m_btnArrow->setChecked(false);
    m_btnCannon->setChecked(true);
    m_btnIce->setChecked(false);
    m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false);
    m_scene->selectTowerType(TowerType::Cannon);
}

void MainWindow::onSelectIce()
{
    m_selectedType = TowerType::Ice;
    m_btnArrow->setChecked(false);
    m_btnCannon->setChecked(false);
    m_btnIce->setChecked(true);
    m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false);
    m_scene->selectTowerType(TowerType::Ice);
}

void MainWindow::onSelectPoison()
{
    m_selectedType = TowerType::Poison;
    m_btnArrow->setChecked(false);
    m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false);
    m_btnPoison->setChecked(true);
    m_btnLightning->setChecked(false);
    m_scene->selectTowerType(TowerType::Poison);
}

void MainWindow::onSelectLightning()
{
    m_selectedType = TowerType::Lightning;
    m_btnArrow->setChecked(false);
    m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false);
    m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(true);
    m_btnSun->setChecked(false);
    m_scene->selectTowerType(TowerType::Lightning);
}

void MainWindow::onSelectSun()
{
    m_selectedType = TowerType::Sun;
    m_btnArrow->setChecked(false);
    m_btnCannon->setChecked(false);
    m_btnIce->setChecked(false);
    m_btnPoison->setChecked(false);
    m_btnLightning->setChecked(false);
    m_btnSun->setChecked(true);
    m_scene->selectTowerType(TowerType::Sun);
}

void MainWindow::onStatsChanged()
{
    updateStatusBar();
    updateTowerButtons();
}

void MainWindow::onGameEnded(bool won)
{
    m_btnStart->setEnabled(true);
    m_btnPause->setEnabled(false);
    m_lblInfo->setText(won ? "Victory! All waves cleared!" : "Game Over! Enemies reached the end!");
}

void MainWindow::updateStatusBar()
{
    m_lblGold->setText(QString("<b>Gold:</b> %1").arg(m_scene->gold()));
    m_lblLives->setText(QString("<b>Lives:</b> %1").arg(m_scene->lives()));
    m_lblWave->setText(QString("<b>Wave:</b> %1/%2").arg(m_scene->currentWave()).arg(m_scene->totalWaves()));

    if (m_scene->isRunning()) {
        m_lblInfo->setText(QString("Enemies: %1  |  Click grid to place towers")
            .arg(m_scene->enemiesInWave()));
    }
}

void MainWindow::updateTowerButtons()
{
    int gold = m_scene->gold();
    m_btnArrow->setEnabled(gold >= 40);
    m_btnCannon->setEnabled(gold >= 80);
    m_btnIce->setEnabled(gold >= 60);
    m_btnPoison->setEnabled(gold >= 65);
    m_btnLightning->setEnabled(gold >= 90);
    m_btnSun->setEnabled(gold >= 50);
}