#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "gamescene.h"
#include "mainmenuwidget.h"
#include "levelselectwidget.h"
#include "gameresultwidget.h"

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
    void onStatsChanged();
    void onGameEnded(bool won, int levelId);

    void onStartGame();
    void onLevelSelected(int id, const QString& file);
    void onRetry();
    void onNextLevel();
    void onMenu();
    void onExitToLevelSelect();

private:
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

    int m_currentLevelId = 1;
    QString m_currentLevelFile;
};

#endif // MAINWINDOW_H
