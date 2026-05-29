#include "levelselectwidget.h"
#include "./ui_levelselectwidget.h"
#include "../game/config/datamanager.h"

LevelSelectWidget::LevelSelectWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LevelSelectWidget)
{
    ui->setupUi(this);

    const auto& levels = DataManager::instance().levels();
    for (const auto& lv : levels)
    {
        auto* item = new QListWidgetItem(lv.name);
        item->setData(Qt::UserRole, lv.id);
        item->setData(Qt::UserRole + 1, lv.file);
        item->setSizeHint(QSize(160, 120));
        item->setTextAlignment(Qt::AlignCenter);
        ui->levelList->addItem(item);
    }

    connect(ui->levelList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem* item) {
        int id = item->data(Qt::UserRole).toInt();
        QString file = item->data(Qt::UserRole + 1).toString();
        emit levelSelected(id, file);
    });

    connect(ui->btnBack, &QPushButton::clicked,
            this, &LevelSelectWidget::backClicked);
}

LevelSelectWidget::~LevelSelectWidget()
{
    delete ui;
}
