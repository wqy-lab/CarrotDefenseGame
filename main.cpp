#include <QApplication>
#include <QDebug>
#include <QIcon>
#include "ui/mainwindow.h"
#include "game/config/datamanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("assets/ui/icon.png"));
    app.setApplicationName("CarrotDefense");
    app.setApplicationVersion("1.0");

    // Load shared unit data (towers + enemies)
    if (!DataManager::instance().loadShared("config/shared.json"))
    {
        qCritical() << "Failed to load config/shared.json";
        return 1;
    }

    // Load level index first to know available levels
    if (!DataManager::instance().loadLevelsIndex("config/levels.json"))
    {
        qCritical() << "Failed to load config/levels.json";
        return 1;
    }

    // Load first available level from the index
    const auto& levels = DataManager::instance().levels();
    if (levels.empty())
    {
        qCritical() << "No levels found in config/levels.json";
        return 1;
    }
    if (!DataManager::instance().loadLevel(levels[0].file))
    {
        qCritical() << "Failed to load" << levels[0].file;
        return 1;
    }

    // Dark theme style
    app.setStyleSheet(
        "QMainWindow { background-color: #2E2E2E; }"
        "QToolBar { background-color: #3C3C3C; border-bottom: 1px solid #555; "
        "spacing: 4px; padding: 2px; }"
        "QPushButton { background-color: #555; color: #EEE; padding: 5px 12px; "
        "border: 1px solid #666; border-radius: 3px; }"
        "QPushButton:hover { background-color: #666; }"
        "QPushButton:disabled { background-color: #444; color: #888; }"
        "QLabel { color: #CCC; }"
        "QStatusBar { background-color: #3C3C3C; color: #AAA; "
        "border-top: 1px solid #555; }"
        "QStatusBar QLabel { padding: 0 8px; }"
    );

    MainWindow w;
    w.show();

    return app.exec();
}
