#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>

#include "gamescene.h"
#include "mainmenuwidget.h"
#include "levelselectwidget.h"
#include "gameresultwidget.h"
#include "../game/towers/tower.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onStartWave();
    void onPauseResume();
    void onSelectArrow();
    void onSelectCannon();
    void onSelectIce();
    void onSelectPoison();
    void onSelectLightning();
    void onSelectSun();
    void onStatsChanged();
    void onGameEnded(bool won, int levelId);

    void onStartGame();
    void onLevelSelected(int id, const QString& file);
    void onRetry();
    void onNextLevel();
    void onMenu();

private:
    void setupToolbar();
    void setupStatusBar();
    void updateStatusBar();
    void updateTowerButtons();
    void loadAndStartLevel(int id, const QString& file);
    void showGamePage();
    void showMenuPage();
    void cleanupOverlay();

    Ui::MainWindow* ui;
    QStackedWidget* m_stacked;

    MainMenuWidget* m_menu;
    LevelSelectWidget* m_levelSelect;
    GameScene* m_scene;
    GameResultWidget* m_resultOverlay = nullptr;

    QPushButton* m_btnStart;
    QPushButton* m_btnPause;
    QPushButton* m_btnArrow;
    QPushButton* m_btnCannon;
    QPushButton* m_btnIce;
    QPushButton* m_btnPoison;
    QPushButton* m_btnLightning;
    QPushButton* m_btnSun;

    QLabel* m_lblGold;
    QLabel* m_lblLives;
    QLabel* m_lblWave;
    QLabel* m_lblInfo;

    TowerType m_selectedType;
    int m_currentLevelId = 1;
    QString m_currentLevelFile;
};

#endif // MAINWINDOW_H
