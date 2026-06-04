#ifndef GAMEOVERLAY_H
#define GAMEOVERLAY_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class GameOverlay;
}
QT_END_NAMESPACE

class GameOverlay : public QWidget {
    Q_OBJECT

public:
    enum class State { Hidden, PauseMenu, ConfirmExit, Message };

    explicit GameOverlay(QWidget* parent = nullptr);
    ~GameOverlay() override;

    void showPauseMenu();
    void showMessage(const QString& title, const QString& content, const QString& buttonText);
    void hideOverlay();
    State currentState() const { return m_state; }

signals:
    void continueClicked();
    void exitToLevelSelectConfirmed();
    void messageDismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* event) override;

private:
    void switchToPage(QWidget* page, State state);

    Ui::GameOverlay* ui = nullptr;
    State m_state = State::Hidden;
};

#endif // GAMEOVERLAY_H
