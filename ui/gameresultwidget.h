#ifndef GAMERESULTWIDGET_H
#define GAMERESULTWIDGET_H

#include <QWidget>

namespace Ui {
class GameResultWidget;
}

class GameResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameResultWidget(bool won, int levelId,
                              QWidget* parent = nullptr);
    ~GameResultWidget();

signals:
    void retryClicked();
    void nextLevelClicked();
    void menuClicked();

private:
    Ui::GameResultWidget* ui;
};

#endif // GAMERESULTWIDGET_H
