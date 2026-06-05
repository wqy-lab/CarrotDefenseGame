#ifndef TUTORIALCONTROLLER_H
#define TUTORIALCONTROLLER_H

#include <QObject>
#include <QMouseEvent>
#include <QPoint>
#include <QString>
#include "tutorialarrow.h"

class QWidget;
class GameScene;
class GameHUD;
class SpatialGrid;
class TowerManager;
class GameOverlay;

enum class CompletionType {
    ClickContinue,
    PlaceTower,
    ClickCell,
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
                                GameHUD* hud,
                                SpatialGrid* grid,
                                TowerManager* tm,
                                GameOverlay* overlay,
                                QWidget* parentWidget,
                                QObject* parent = nullptr);
    ~TutorialController() = default;

    void start();
    bool isActive() const { return m_active; }
    bool checkCompletion(QMouseEvent* event);
    void onEnemyKilled();

private slots:
    void nextStep();

private:
    void updateOverlay();
    void updateArrow();
    QPoint arrowTargetPoint() const;
    QPoint arrowFromPoint() const;

    GameScene* m_scene = nullptr;
    GameHUD* m_hud = nullptr;
    SpatialGrid* m_grid = nullptr;
    TowerManager* m_tm = nullptr;
    GameOverlay* m_overlay = nullptr;
    QWidget* m_parentWidget = nullptr;
    TutorialArrow* m_arrow = nullptr;

    TutorialStep m_steps[8];
    int m_currentStep = -1;
    bool m_active = false;
    bool m_killDetected = false;
    bool m_waitingForContinue = true;
};

#endif // TUTORIALCONTROLLER_H
