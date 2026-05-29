#include "levelselectwidget.h"
#include "./ui_levelselectwidget.h"
#include "../game/config/datamanager.h"

LevelSelectWidget::LevelSelectWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LevelSelectWidget)
{
    ui->setupUi(this);

    const auto& levels = DataManager::instance().levels();
    int index = 0;
    for (const auto& lv : levels)
    {
        auto* item = new QListWidgetItem(lv.name);
        item->setData(Qt::UserRole, lv.id);
        item->setData(Qt::UserRole + 1, lv.file);
        item->setSizeHint(QSize(160, 120));
        item->setTextAlignment(Qt::AlignCenter);

#ifndef QT_DEBUG
        if (index > 0)
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
#endif

        ui->levelList->addItem(item);
        ++index;
    }

    connect(ui->levelList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem* item) {
        if (!(item->flags() & Qt::ItemIsEnabled)) return;
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

void LevelSelectWidget::unlockLevel(int id)
{
    for (int i = 0; i < ui->levelList->count(); ++i)
    {
        auto* item = ui->levelList->item(i);
        if (item->data(Qt::UserRole).toInt() == id)
        {
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
            break;
        }
    }
}
