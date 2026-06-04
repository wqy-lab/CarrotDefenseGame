#include "gameoverlay.h"
#include "./ui_gameoverlay.h"

#include <QPainter>
#include <QMouseEvent>

GameOverlay::GameOverlay(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GameOverlay)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_NoSystemBackground, false);
    hide();

    // ── Connections ──
    connect(ui->btnContinue, &QPushButton::clicked,
            this, &GameOverlay::continueClicked);
    connect(ui->btnExit, &QPushButton::clicked, this, [this]() {
        switchToPage(ui->confirmPage, State::ConfirmExit);
    });
    connect(ui->btnConfirm, &QPushButton::clicked,
            this, &GameOverlay::exitToLevelSelectConfirmed);
    connect(ui->btnCancel, &QPushButton::clicked, this, [this]() {
        switchToPage(ui->pausePage, State::PauseMenu);
    });
    connect(ui->btnMessageDismiss, &QPushButton::clicked,
            this, &GameOverlay::messageDismissed);
}

GameOverlay::~GameOverlay()
{
    delete ui;
}

void GameOverlay::switchToPage(QWidget* page, State state)
{
    m_state = state;
    ui->stack->setCurrentWidget(page);
}

void GameOverlay::showPauseMenu()
{
    switchToPage(ui->pausePage, State::PauseMenu);
    show();
    raise();
}

void GameOverlay::showMessage(const QString& title, const QString& content, const QString& buttonText)
{
    ui->messageTitle->setText(title);
    ui->messageContent->setText(content);
    ui->btnMessageDismiss->setText(buttonText);
    switchToPage(ui->messagePage, State::Message);
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
    ui->stack->setGeometry(rect());
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
