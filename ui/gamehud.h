#ifndef GAMEHUD_H
#define GAMEHUD_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class GameHUD;
}
QT_END_NAMESPACE

class GameHUD : public QWidget {
    Q_OBJECT

public:
    explicit GameHUD(QWidget* parent = nullptr);
    ~GameHUD() override;

    void setGold(int gold);
    void setLives(int lives);
    void setWave(int current, int total);
    void setEnemies(int count);
    void setPausedState(bool isPaused);

signals:
    void pauseClicked();

private:
    Ui::GameHUD* ui = nullptr;
};

#endif // GAMEHUD_H
