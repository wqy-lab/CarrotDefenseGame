#ifndef TOWERPANEL_H
#define TOWERPANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class Tower;

class TowerPanel : public QWidget {
    Q_OBJECT

public:
    explicit TowerPanel(QWidget* parent = nullptr);
    void setTower(Tower* tower);
    void updateInfo();

public slots:
    void updateGold(int gold);

signals:
    void upgradeClicked();
    void sellClicked();
    void panelHidden();
    void upgradeHovered(bool hovered);
    void sellHovered(bool hovered);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

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
};

#endif