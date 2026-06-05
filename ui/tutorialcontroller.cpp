#include "tutorialcontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "../game/gamecontroller.h"
#include "gamescene.h"
#include "gamehud.h"
#include "gameoverlay.h"
#include <QtMath>

TutorialController::TutorialController(GameScene* scene,
                                       GameHUD* hud,
                                       SpatialGrid* grid,
                                       TowerManager* tm,
                                       GameOverlay* overlay,
                                       QWidget* parentWidget,
                                       QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_hud(hud)
    , m_grid(grid)
    , m_tm(tm)
    , m_overlay(overlay)
    , m_parentWidget(parentWidget)
{
    m_arrow = new TutorialArrow(parentWidget);
    m_arrow->setGeometry(parentWidget->rect());
    m_arrow->hide();

    // ── Step 0 ──
    m_steps[0].text = QString::fromUtf8(
        "欢迎来到胡萝卜塔防！\n\n"
        "敌人会沿道路从左向右前进，\n"
        "你的目标是阻止它们到达右边的萝卜。");
    m_steps[0].showArrow = true;
    m_steps[0].arrowDir = ArrowDirection::Down;
    m_steps[0].arrowTargetX = m_grid->endX();
    m_steps[0].arrowTargetY = m_grid->endY();
    m_steps[0].completion = CompletionType::ClickContinue;

    // ── Step 1 ──
    m_steps[1].text = QString::fromUtf8(
        "这是暂停按钮。\n"
        "点击它可以随时暂停或继续游戏，\n"
        "也可以按键盘的 Esc 键。");
    m_steps[1].showArrow = true;
    m_steps[1].arrowDir = ArrowDirection::Up;
    m_steps[1].completion = CompletionType::ClickContinue;

    // ── Step 2 ──
    m_steps[2].text = QString::fromUtf8(
        "这里显示了你的金币、生命值、\n"
        "当前波次和场上敌人数量。\n"
        "随时关注这些信息！");
    m_steps[2].showArrow = true;
    m_steps[2].arrowDir = ArrowDirection::Up;
    m_steps[2].completion = CompletionType::ClickContinue;

    // ── Step 3 ──
    m_steps[3].text = QString::fromUtf8(
        "点击这个空地来放置一座防御塔。\n"
        "试试看，点击箭头指着的绿色格子。");
    m_steps[3].showArrow = true;
    m_steps[3].arrowDir = ArrowDirection::Down;
    m_steps[3].arrowTargetX = 7;
    m_steps[3].arrowTargetY = 6;
    m_steps[3].completion = CompletionType::PlaceTower;
    m_steps[3].paramX = 7;
    m_steps[3].paramY = 6;

    // ── Step 4 ──
    m_steps[4].text = QString::fromUtf8(
        "敌人来了！观察你的塔如何自动攻击。\n"
        "塔会优先攻击范围内的敌人。");
    m_steps[4].showArrow = false;
    m_steps[4].completion = CompletionType::EnemyKilled;

    // ── Step 5 ──
    m_steps[5].text = QString::fromUtf8(
        "点击你刚刚放置的塔，\n"
        "可以升级或出售它。\n"
        "升级后的塔攻击力更强！");
    m_steps[5].showArrow = true;
    m_steps[5].arrowDir = ArrowDirection::Down;
    m_steps[5].arrowTargetX = 7;
    m_steps[5].arrowTargetY = 6;
    m_steps[5].completion = CompletionType::ClickCell;
    m_steps[5].paramX = 7;
    m_steps[5].paramY = 6;

    // ── Step 6 ──
    m_steps[6].text = QString::fromUtf8(
        "点击障碍物可以将其设为攻击目标。\n"
        "摧毁后获得金币并腾出空地放置新塔！");
    m_steps[6].showArrow = true;
    m_steps[6].arrowDir = ArrowDirection::Down;
    m_steps[6].arrowTargetX = 3;
    m_steps[6].arrowTargetY = 4;
    m_steps[6].completion = CompletionType::ClickCell;
    m_steps[6].paramX = 3;
    m_steps[6].paramY = 4;

    // ── Step 7 ──
    m_steps[7].text = QString::fromUtf8(
        "你已经掌握了基本操作！\n"
        "祝你好运，保卫萝卜！");
    m_steps[7].showArrow = false;
    m_steps[7].completion = CompletionType::ClickContinue;
}

void TutorialController::start()
{
    m_active = true;
    m_currentStep = 0;
    m_killDetected = false;
    m_waitingForContinue = true;

    m_scene->pauseGame();
    updateOverlay();
    updateArrow();

    connect(m_overlay, &GameOverlay::messageDismissed,
            this, &TutorialController::nextStep,
            Qt::UniqueConnection);
}

void TutorialController::nextStep()
{
    if (!m_active) return;

    m_currentStep++;
    if (m_currentStep >= 8) {
        // Tutorial complete
        m_active = false;
        m_arrow->hide();
        m_overlay->hideOverlay();
        disconnect(m_overlay, &GameOverlay::messageDismissed,
                   this, &TutorialController::nextStep);
        return;
    }

    auto& step = m_steps[m_currentStep];

    if (step.completion == CompletionType::EnemyKilled) {
        // Step 4: resume game, hide everything, wait for kill
        m_waitingForContinue = false;
        m_arrow->hide();
        m_overlay->hideOverlay();
        m_killDetected = false;
        m_scene->resumeGame();
        return;
    }

    // All other steps: pause game, show overlay
    m_scene->pauseGame();
    m_waitingForContinue = (step.completion == CompletionType::ClickContinue);
    updateOverlay();
    updateArrow();
}

void TutorialController::updateOverlay()
{
    if (m_currentStep < 0 || m_currentStep >= 8) return;
    auto& step = m_steps[m_currentStep];
    m_overlay->showMessage(QString::fromUtf8("新手教程"),
                           step.text,
                           QString::fromUtf8("继续"));
}

void TutorialController::updateArrow()
{
    if (m_currentStep < 0 || m_currentStep >= 8) return;
    auto& step = m_steps[m_currentStep];

    if (!step.showArrow) {
        m_arrow->hide();
        return;
    }

    QPoint from = arrowFromPoint();
    QPoint to = arrowTargetPoint();
    m_arrow->setTarget(from, to, step.arrowDir);
    m_arrow->show();
    m_arrow->raise();
}

QPoint TutorialController::arrowFromPoint() const
{
    auto& step = m_steps[m_currentStep];
    // Arrow starts from the center-top or center-bottom of the overlay
    QPoint overlayCenter(m_parentWidget->width() / 2,
                         step.arrowDir == ArrowDirection::Up
                             ? m_parentWidget->height() * 3 / 4
                             : m_parentWidget->height() / 4);
    return overlayCenter;
}

QPoint TutorialController::arrowTargetPoint() const
{
    auto& step = m_steps[m_currentStep];

    // HUD targets
    if (step.arrowDir == ArrowDirection::Up) {
        int hudY = m_hud->height() / 2;
        // Approximate center of the HUD info area for steps 1-2
        int targetX = m_parentWidget->width() / 2;
        return QPoint(targetX, hudY);
    }

    // Map targets
    if (step.arrowTargetX >= 0 && step.arrowTargetY >= 0) {
        QPointF cell = m_grid->gridToPixel(step.arrowTargetX, step.arrowTargetY);
        return QPoint(static_cast<int>(cell.x()),
                      static_cast<int>(cell.y() - m_grid->cellSize() * 0.5));
    }

    return QPoint(m_parentWidget->width() / 2, m_parentWidget->height() / 2);
}

bool TutorialController::checkCompletion(QMouseEvent* event)
{
    if (!m_active || m_waitingForContinue) return false;

    auto& step = m_steps[m_currentStep];

    if (step.completion == CompletionType::PlaceTower) {
        int gx = static_cast<int>(std::floor((event->pos().x() - m_grid->offsetX())
                                             / m_grid->cellSize()));
        int gy = static_cast<int>(std::floor((event->pos().y() - m_grid->offsetY())
                                             / m_grid->cellSize()));
        if (gx == step.paramX && gy == step.paramY
            && m_tm->getTowerAt(gx, gy) != nullptr) {
            nextStep();
            return true;
        }
    }

    if (step.completion == CompletionType::ClickCell) {
        double gx = (event->pos().x() - m_grid->offsetX()) / m_grid->cellSize();
        double gy = (event->pos().y() - m_grid->offsetY()) / m_grid->cellSize();
        if (std::abs(gx - step.paramX) < 1.5
            && std::abs(gy - step.paramY) < 1.5) {
            nextStep();
            return true;
        }
    }

    return false;
}

void TutorialController::onEnemyKilled()
{
    if (!m_active) return;
    auto& step = m_steps[m_currentStep];
    if (step.completion == CompletionType::EnemyKilled && !m_killDetected) {
        m_killDetected = true;
        // Small delay before showing next step
        m_scene->pauseGame();
        m_waitingForContinue = false;
        nextStep();
    }
}
