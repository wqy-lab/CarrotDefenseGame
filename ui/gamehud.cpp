#include "gamehud.h"
#include "./ui_gamehud.h"

GameHUD::GameHUD(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GameHUD)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    connect(ui->btnPause, &QPushButton::clicked,
            this, &GameHUD::pauseClicked);
}

GameHUD::~GameHUD()
{
    delete ui;
}

void GameHUD::setGold(int gold)
{
    ui->lblGold->setText(QString::number(gold));
}

void GameHUD::setLives(int lives)
{
    ui->lblLives->setText(QString::number(lives));
}

void GameHUD::setWave(int current, int total)
{
    ui->lblWave->setText(QString("%1/%2").arg(current).arg(total));
}

void GameHUD::setEnemies(int count)
{
    ui->lblEnemies->setText(QString::number(count));
}

void GameHUD::setPausedState(bool isPaused)
{
    ui->btnPause->setText(isPaused ? QString::fromUtf8("\u25B6 Resume")
                                   : QString::fromUtf8("\u23F8 Pause"));
}
