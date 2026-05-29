#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>

namespace Ui {
class MainMenuWidget;
}

class MainMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget* parent = nullptr);
    ~MainMenuWidget();

signals:
    void startGameClicked();
    void levelSelectClicked();
    void exitClicked();

private:
    Ui::MainMenuWidget* ui;
};

#endif // MAINMENUWIDGET_H
