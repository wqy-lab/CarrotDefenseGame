#include "gameoverlay.h"

#include <QPainter>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>

static const char* kButtonStyle = R"(
    QPushButton {
        background-color: #4CAF50;
        color: white;
        font-size: 16px;
        font-weight: bold;
        border: none;
        border-radius: 6px;
        padding: 10px 24px;
        min-width: 200px;
    }
    QPushButton:hover {
        background-color: #45a049;
    }
)";

static const char* kExitButtonStyle = R"(
    QPushButton {
        background-color: #C62828;
        color: white;
        font-size: 16px;
        font-weight: bold;
        border: none;
        border-radius: 6px;
        padding: 10px 24px;
        min-width: 200px;
    }
    QPushButton:hover {
        background-color: #D32F2F;
    }
)";

static const char* kCancelButtonStyle = R"(
    QPushButton {
        background-color: #555;
        color: #CCC;
        font-size: 16px;
        font-weight: bold;
        border: 1px solid #666;
        border-radius: 6px;
        padding: 10px 24px;
        min-width: 120px;
    }
    QPushButton:hover {
        background-color: #666;
    }
)";

static const char* kTitleStyle = R"(
    QLabel {
        color: white;
        font-size: 36px;
        font-weight: bold;
    }
)";

static const char* kContentStyle = R"(
    QLabel {
        color: #CCC;
        font-size: 16px;
    }
)";

GameOverlay::GameOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground, false);
    setupUI();
    hide();
}

void GameOverlay::setupUI()
{
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->setStyleSheet("QStackedWidget { background: transparent; }");
    rootLayout->addWidget(m_stack);

    // ── Pause Page ──
    {
        m_pausePage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_pausePage);
        layout->setAlignment(Qt::AlignCenter);
        layout->setSpacing(20);

        m_pauseTitle = new QLabel("游戏暂停");
        m_pauseTitle->setAlignment(Qt::AlignCenter);
        m_pauseTitle->setStyleSheet(kTitleStyle);
        layout->addWidget(m_pauseTitle);

        layout->addSpacing(10);

        QVBoxLayout* btnLayout = new QVBoxLayout();
        btnLayout->setAlignment(Qt::AlignCenter);
        btnLayout->setSpacing(12);

        m_btnContinue = new QPushButton("继续游戏");
        m_btnContinue->setCursor(Qt::PointingHandCursor);
        m_btnContinue->setStyleSheet(kButtonStyle);
        btnLayout->addWidget(m_btnContinue, 0, Qt::AlignCenter);

        m_btnExit = new QPushButton("返回选关");
        m_btnExit->setCursor(Qt::PointingHandCursor);
        m_btnExit->setStyleSheet(kExitButtonStyle);
        btnLayout->addWidget(m_btnExit, 0, Qt::AlignCenter);

        layout->addLayout(btnLayout);
        m_stack->addWidget(m_pausePage);
    }

    // ── Confirm Exit Page ──
    {
        m_confirmPage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_confirmPage);
        layout->setAlignment(Qt::AlignCenter);
        layout->setSpacing(14);

        m_confirmTitle = new QLabel("退出关卡？");
        m_confirmTitle->setAlignment(Qt::AlignCenter);
        m_confirmTitle->setStyleSheet(kTitleStyle);
        layout->addWidget(m_confirmTitle);

        m_confirmText = new QLabel("确定要退出当前关卡吗？\n将返回选关界面");
        m_confirmText->setAlignment(Qt::AlignCenter);
        m_confirmText->setStyleSheet(kContentStyle);
        layout->addWidget(m_confirmText);

        layout->addSpacing(6);

        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->setAlignment(Qt::AlignCenter);
        btnLayout->setSpacing(20);

        m_btnConfirm = new QPushButton("确认");
        m_btnConfirm->setCursor(Qt::PointingHandCursor);
        m_btnConfirm->setStyleSheet(kExitButtonStyle);
        m_btnConfirm->setMinimumWidth(140);
        btnLayout->addWidget(m_btnConfirm);

        m_btnCancel = new QPushButton("取消");
        m_btnCancel->setCursor(Qt::PointingHandCursor);
        m_btnCancel->setStyleSheet(kCancelButtonStyle);
        m_btnCancel->setMinimumWidth(140);
        btnLayout->addWidget(m_btnCancel);

        layout->addLayout(btnLayout);
        m_stack->addWidget(m_confirmPage);
    }

    // ── Message Page (future use) ──
    {
        m_messagePage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_messagePage);
        layout->setAlignment(Qt::AlignCenter);
        layout->setSpacing(16);

        m_messageTitle = new QLabel();
        m_messageTitle->setAlignment(Qt::AlignCenter);
        m_messageTitle->setStyleSheet(kTitleStyle);
        layout->addWidget(m_messageTitle);

        m_messageContent = new QLabel();
        m_messageContent->setAlignment(Qt::AlignCenter);
        m_messageContent->setWordWrap(true);
        m_messageContent->setStyleSheet(kContentStyle);
        layout->addWidget(m_messageContent);

        layout->addSpacing(4);

        m_btnMessageDismiss = new QPushButton("我知道了");
        m_btnMessageDismiss->setCursor(Qt::PointingHandCursor);
        m_btnMessageDismiss->setStyleSheet(kButtonStyle);
        layout->addWidget(m_btnMessageDismiss, 0, Qt::AlignCenter);

        m_stack->addWidget(m_messagePage);
    }

    // ── Connections ──
    connect(m_btnContinue, &QPushButton::clicked, this, &GameOverlay::continueClicked);
    connect(m_btnExit, &QPushButton::clicked, this, [this]() {
        switchToPage(m_confirmPage, State::ConfirmExit);
    });
    connect(m_btnConfirm, &QPushButton::clicked, this, &GameOverlay::exitToLevelSelectConfirmed);
    connect(m_btnCancel, &QPushButton::clicked, this, [this]() {
        switchToPage(m_pausePage, State::PauseMenu);
    });
    connect(m_btnMessageDismiss, &QPushButton::clicked, this, &GameOverlay::messageDismissed);
}

void GameOverlay::switchToPage(QWidget* page, State state)
{
    m_state = state;
    m_stack->setCurrentWidget(page);
}

void GameOverlay::showPauseMenu()
{
    switchToPage(m_pausePage, State::PauseMenu);
    show();
    raise();
}

void GameOverlay::showMessage(const QString& title, const QString& content, const QString& buttonText)
{
    m_messageTitle->setText(title);
    m_messageContent->setText(content);
    m_btnMessageDismiss->setText(buttonText);
    switchToPage(m_messagePage, State::Message);
    show();
    raise();
}

void GameOverlay::hideOverlay()
{
    m_state = State::Hidden;
    hide();
}

void GameOverlay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 180));
}

void GameOverlay::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_stack) {
        m_stack->setGeometry(rect());
    }
}

bool GameOverlay::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        return true;
    default:
        break;
    }
    return QWidget::event(event);
}
