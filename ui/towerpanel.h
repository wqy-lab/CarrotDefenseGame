#ifndef TOWERPANEL_H
#define TOWERPANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>

class Tower;

class TowerPanel : public QWidget {
    Q_OBJECT

public:
    explicit TowerPanel(QWidget* parent = nullptr);
    void setTower(Tower* tower);
    void updateInfo();

signals:
    void upgradeClicked();
    void sellClicked();

protected:
    void showEvent(QShowEvent* event) override;
    bool event(QEvent* event) override;

private slots:
    void onUpgradeClicked();
    void onSellClicked();

private:
    Tower* m_tower = nullptr;
    QLabel* m_lblLevel;
    QLabel* m_lblName;
    QLabel* m_lblStats;
    QPushButton* m_btnUpgrade;
    QPushButton* m_btnSell;
    QPropertyAnimation* m_scaleAnim;
};

#endif