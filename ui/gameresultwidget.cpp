#include "gameresultwidget.h"
#include "./ui_gameresultwidget.h"

GameResultWidget::GameResultWidget(bool won, int levelId,
                                   QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GameResultWidget)
{
    ui->setupUi(this);

    ui->resultLabel->setText(won ? "VICTORY!" : "GAME OVER");
    ui->resultLabel->setStyleSheet(
        won ? "font-size: 48px; font-weight: bold; color: #FF9800;"
            : "font-size: 48px; font-weight: bold; color: #f44336;");

    if (!won)
        ui->btnNext->hide();

    connect(ui->btnRetry, &QPushButton::clicked,
            this, &GameResultWidget::retryClicked);
    connect(ui->btnNext, &QPushButton::clicked,
            this, &GameResultWidget::nextLevelClicked);
    connect(ui->btnMenu, &QPushButton::clicked,
            this, &GameResultWidget::menuClicked);
}

GameResultWidget::~GameResultWidget()
{
    delete ui;
}
