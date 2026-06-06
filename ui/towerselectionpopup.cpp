#include "towerselectionpopup.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QToolTip>

TowerSelectionPopup::TowerSelectionPopup(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFixedSize(260, 200);
    setStyleSheet("QWidget { background-color: rgba(30, 35, 30, 230); border: 2px solid rgba(100, 120, 80, 180); border-radius: 8px; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(6);

    QLabel* titleLabel = new QLabel("Select Tower", this);
    titleLabel->setStyleSheet("QLabel { color: white; font-size: 14px; font-weight: bold; padding: 4px; }");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Tower buttons grid
    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(6);

    createTowerButton(TowerType::Arrow, "Arrow", QColor(100, 180, 100), 50);
    createTowerButton(TowerType::Cannon, "Cannon", QColor(150, 100, 60), 80);
    createTowerButton(TowerType::Ice, "Ice", QColor(150, 200, 255), 70);
    createTowerButton(TowerType::Poison, "Poison", QColor(150, 80, 180), 90);
    createTowerButton(TowerType::Lightning, "Lightning", QColor(255, 220, 100), 120);
    createTowerButton(TowerType::Sun, "Sun", QColor(255, 180, 50), 100);

    mainLayout->addLayout(m_gridLayout);
    mainLayout->addStretch();
}

void TowerSelectionPopup::createTowerButton(TowerType type, const QString& name, const QColor& color, int cost)
{
    QPushButton* btn = new QPushButton(this);
    btn->setFixedSize(75, 50);
    btn->setCursor(Qt::PointingHandCursor);

    m_costs[type] = cost;

    // Style based on color
    QString qss = QString(
        "QPushButton { "
        "  background-color: %1; "
        "  border: 2px solid rgba(0,0,0,100); "
        "  border-radius: 6px; "
        "  color: white; "
        "  font-weight: bold; "
        "  font-size: 11px; "
        "  padding: 2px; "
        "} "
        "QPushButton:hover { "
        "  background-color: %2; "
        "  border: 2px solid white; "
        "} "
        "QPushButton:disabled { "
        "  background-color: gray; "
        "  color: rgba(255,255,255,100); "
        "  border: 2px solid rgba(0,0,0,50); "
        "}"
    ).arg(color.name()).arg(color.darker(110).name());

    btn->setStyleSheet(qss);
    btn->setText(QString("%1\n%2G").arg(name).arg(cost));

    m_buttons[type] = btn;

    btn->installEventFilter(this);

    int col = m_buttons.keys().indexOf(type) % 3;
    int row = m_buttons.keys().indexOf(type) / 3;

    m_gridLayout->addWidget(btn, row, col);

    connect(btn, &QPushButton::clicked, this, [this, type]() { onTowerClicked(type); });
}

void TowerSelectionPopup::onTowerClicked(TowerType type)
{
    if (m_buttons.value(type)->isEnabled()) {
        emit towerSelected(type);
    }
}

void TowerSelectionPopup::updateGold(int gold)
{
    m_currentGold = gold;
    for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it) {
        int cost = m_costs.value(it.key(), 0);
        it.value()->setEnabled(gold >= cost);
    }
}

void TowerSelectionPopup::setTowerEnabled(TowerType type, bool enabled)
{
    if (m_buttons.contains(type)) {
        m_buttons[type]->setEnabled(enabled);
    }
}

void TowerSelectionPopup::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
}

void TowerSelectionPopup::hideEvent(QHideEvent* event)
{
    emit cancelled();
    QWidget::hideEvent(event);
}

bool TowerSelectionPopup::event(QEvent* event)
{
    if (event->type() == QEvent::FocusOut || event->type() == QEvent::MouseButtonPress) {
        hide();
        return true;
    }
    return QWidget::event(event);
}

bool TowerSelectionPopup::eventFilter(QObject* obj, QEvent* event)
{
    for (auto it = m_buttons.begin(); it != m_buttons.end(); ++it) {
        if (obj == it.value()) {
            if (event->type() == QEvent::Enter) {
                emit towerButtonHovered(it.key(), true);
            } else if (event->type() == QEvent::Leave) {
                emit towerButtonHovered(it.key(), false);
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}