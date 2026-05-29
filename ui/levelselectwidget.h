#ifndef LEVELSELECTWIDGET_H
#define LEVELSELECTWIDGET_H

#include <QWidget>

namespace Ui {
class LevelSelectWidget;
}

class LevelSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LevelSelectWidget(QWidget* parent = nullptr);
    ~LevelSelectWidget();

    void unlockLevel(int id);

signals:
    void backClicked();
    void levelSelected(int id, const QString& file);

private:
    Ui::LevelSelectWidget* ui;
};

#endif // LEVELSELECTWIDGET_H
