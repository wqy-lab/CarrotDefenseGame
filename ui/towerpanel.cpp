#include "towerpanel.h"
#include "../game/towers/tower.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>
#include <QEvent>

TowerPanel::TowerPanel(QWidget* parent)
    : QWidget(parent)
    , m_tower(nullptr)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFixedSize(180, 140);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(4);

    m_lblLevel = new QLabel(this);
    m_lblName = new QLabel(this);
    m_lblStats = new QLabel(this);

    m_lblLevel->setStyleSheet("color: #FFD700; font-size: 14px; font-weight: bold;");
    m_lblName->setStyleSheet("color: #FFFFFF; font-size: 12px;");
    m_lblStats->setStyleSheet("color: #AAAAAA; font-size: 11px;");

    m_btnUpgrade = new QPushButton("Upgrade", this);
    m_btnSell = new QPushButton("Sell", this);

    m_btnUpgrade->setFixedHeight(28);
    m_btnSell->setFixedHeight(28);

    m_btnUpgrade->setStyleSheet(R"(
        QPushButton {
            background-color: #2E7D32;
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #388E3C; }
        QPushButton:disabled { background-color: #555; color: #888; }
    )");

    m_btnSell->setStyleSheet(R"(
        QPushButton {
            background-color: #C62828;
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #D32F2F; }
    )");

    mainLayout->addWidget(m_lblLevel);
    mainLayout->addWidget(m_lblName);
    mainLayout->addWidget(m_lblStats);
    mainLayout->addSpacing(4);
    mainLayout->addWidget(m_btnUpgrade);
    mainLayout->addWidget(m_btnSell);

    setStyleSheet(R"(
        TowerPanel {
            background-color: rgba(30, 30, 40, 220);
            border: 1px solid #555;
            border-radius: 8px;
        }
    )");

    connect(m_btnUpgrade, &QPushButton::clicked, this, &TowerPanel::onUpgradeClicked);
    connect(m_btnSell, &QPushButton::clicked, this, &TowerPanel::onSellClicked);

    m_btnUpgrade->installEventFilter(this);
    m_btnSell->installEventFilter(this);
}

void TowerPanel::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (m_tower) updateInfo();
}

void TowerPanel::hideEvent(QHideEvent* event) {
    emit panelHidden();
    QWidget::hideEvent(event);
}

bool TowerPanel::event(QEvent* event) {
    if (event->type() == QEvent::FocusOut) {
        hide();
        return true;
    }
    return QWidget::event(event);
}

bool TowerPanel::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Enter) {
        if (obj == m_btnUpgrade) emit upgradeHovered(true);
        else if (obj == m_btnSell) emit sellHovered(true);
    } else if (event->type() == QEvent::Leave) {
        if (obj == m_btnUpgrade) emit upgradeHovered(false);
        else if (obj == m_btnSell) emit sellHovered(false);
    }
    return QWidget::eventFilter(obj, event);
}

void TowerPanel::setTower(Tower* tower) {
    m_tower = tower;
    updateInfo();
}

void TowerPanel::updateInfo() {
    if (!m_tower) return;

    m_lblLevel->setText(QString("Lvl %1").arg(m_tower->level()));
    m_lblName->setText(m_tower->name());

    m_lblStats->setText(QString("DMG: %1  RNG: %2").arg(m_tower->damage(), 0, 'f', 1).arg(m_tower->range(), 0, 'f', 1));

    if (m_tower->level() >= Tower::MAX_LEVEL) {
        m_btnUpgrade->setText("MAX");
        m_btnUpgrade->setEnabled(false);
    } else {
        int upgradeCost = m_tower->upgradeCost();
        m_btnUpgrade->setText(QString("Upgrade: %1g").arg(upgradeCost));
        m_btnUpgrade->setEnabled(true);
    }

    int sellValue = m_tower->sellValue();
    m_btnSell->setText(QString("Sell: %1g").arg(sellValue));
}

void TowerPanel::updateGold(int gold) {
    if (!m_tower) return;
    if (m_tower->level() >= Tower::MAX_LEVEL) return;
    int upgradeCost = m_tower->upgradeCost();
    m_btnUpgrade->setEnabled(gold >= upgradeCost);
}

void TowerPanel::onUpgradeClicked() {
    emit upgradeClicked();
}

void TowerPanel::onSellClicked() {
    emit sellClicked();
}