#include "mainmenuwidget.h"
#include "./ui_mainmenuwidget.h"
#include <QApplication>

MainMenuWidget::MainMenuWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainMenuWidget)
{
    ui->setupUi(this);

    connect(ui->btnStart, &QPushButton::clicked,
            this, &MainMenuWidget::startGameClicked);
    connect(ui->btnLevelSelect, &QPushButton::clicked,
            this, &MainMenuWidget::levelSelectClicked);
    connect(ui->btnExit, &QPushButton::clicked,
            qApp, &QApplication::quit);
}

MainMenuWidget::~MainMenuWidget()
{
    delete ui;
}
