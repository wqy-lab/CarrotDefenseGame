#ifndef GAMEOVERLAY_H
#define GAMEOVERLAY_H

#include <QWidget>

class QStackedWidget;
class QLabel;
class QPushButton;
class QVBoxLayout;

class GameOverlay : public QWidget {
    Q_OBJECT

public:
    enum class State { Hidden, PauseMenu, ConfirmExit, Message };

    explicit GameOverlay(QWidget* parent = nullptr);

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
    void setupUI();
    void switchToPage(QWidget* page, State state);

    QStackedWidget* m_stack = nullptr;

    QWidget* m_pausePage = nullptr;
    QLabel* m_pauseTitle = nullptr;
    QPushButton* m_btnContinue = nullptr;
    QPushButton* m_btnExit = nullptr;

    QWidget* m_confirmPage = nullptr;
    QLabel* m_confirmTitle = nullptr;
    QLabel* m_confirmText = nullptr;
    QPushButton* m_btnConfirm = nullptr;
    QPushButton* m_btnCancel = nullptr;

    QWidget* m_messagePage = nullptr;
    QLabel* m_messageTitle = nullptr;
    QLabel* m_messageContent = nullptr;
    QPushButton* m_btnMessageDismiss = nullptr;

    State m_state = State::Hidden;
};

#endif // GAMEOVERLAY_H
