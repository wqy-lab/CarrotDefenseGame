#ifndef TUTORIALCONTROLLER_H
#define TUTORIALCONTROLLER_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QString>
#include "tutorialarrow.h"

class QWidget;
class GameScene;
class SpatialGrid;
class TowerManager;
class GameController;

enum class CompletionType {
    ClickContinue,
    PlaceTower,
    ClickCell,
    TowerUpgraded,
    EnemyKilled,
};

struct TutorialStep {
    QString text;
    bool showArrow = false;
    ArrowDirection arrowDir = ArrowDirection::Down;
    int arrowTargetX = -1;
    int arrowTargetY = -1;
    CompletionType completion = CompletionType::ClickContinue;
    int paramX = -1;
    int paramY = -1;
};

class TutorialController : public QObject {
    Q_OBJECT

public:
    explicit TutorialController(GameScene* scene,
                                SpatialGrid* grid,
                                TowerManager* tm,
                                GameController* gc,
                                QWidget* parentWidget,
                                QObject* parent = nullptr);
    ~TutorialController() = default;

    void start();
    bool isActive() const { return m_active; }
    void onEnemyKilled();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void nextStep();
    void onArrowClicked();

private:
    void updateArrow();
    void checkAfterInput();
    QPoint arrowFromPoint() const;
    QPoint arrowTargetPoint() const;

    GameScene* m_scene = nullptr;
    SpatialGrid* m_grid = nullptr;
    TowerManager* m_tm = nullptr;
    GameController* m_gc = nullptr;
    QWidget* m_parentWidget = nullptr;
    TutorialArrow* m_arrow = nullptr;

    TutorialStep m_steps[7];
    int m_currentStep = -1;
    bool m_active = false;
    bool m_killDetected = false;

    QMetaObject::Connection m_stepConnection;
};

#endif // TUTORIALCONTROLLER_H
