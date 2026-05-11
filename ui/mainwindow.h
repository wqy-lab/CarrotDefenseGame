#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

#include "gamescene.h"
#include "../game/towers/tower.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
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
    void onGameEnded(bool won);

private:
    void updateStatusBar();
    void updateTowerButtons();

    Ui::MainWindow* ui;
    GameScene* m_scene;

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
};

#endif // MAINWINDOW_H