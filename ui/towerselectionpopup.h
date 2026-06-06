#ifndef TOWERSELECTIONPOPUP_H
#define TOWERSELECTIONPOPUP_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include "../game/towers/tower.h"

class TowerSelectionPopup : public QWidget {
    Q_OBJECT

public:
    explicit TowerSelectionPopup(QWidget* parent = nullptr);

    void setGridPos(int x, int y) { m_gridX = x; m_gridY = y; }
    int gridX() const { return m_gridX; }
    int gridY() const { return m_gridY; }

    void updateGold(int gold);
    void setTowerEnabled(TowerType type, bool enabled);

signals:
    void towerSelected(TowerType type);
    void cancelled();
    void towerButtonHovered(TowerType type, bool hovered);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onTowerClicked(TowerType type);

private:
    void createTowerButton(TowerType type, const QString& name, const QColor& color, int cost);

    int m_gridX = -1;
    int m_gridY = -1;
    int m_currentGold = 0;

    QGridLayout* m_gridLayout = nullptr;
    QMap<TowerType, QPushButton*> m_buttons;
    QMap<TowerType, int> m_costs;
};

#endif