#include "tutorialcontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "../game/gamecontroller.h"
#include "gamescene.h"
#include <QtMath>
#include <QTimer>

TutorialController::TutorialController(GameScene* scene,
                                       SpatialGrid* grid,
                                       TowerManager* tm,
                                       GameController* gc,
                                       QWidget* parentWidget,
                                       QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_grid(grid)
    , m_tm(tm)
    , m_gc(gc)
    , m_parentWidget(parentWidget)
{
    m_arrow = new TutorialArrow(parentWidget);
    m_arrow->setGeometry(parentWidget->rect());
    m_arrow->hide();

    // ── Step 0 ──
    m_steps[0].text = QString::fromUtf8(
        "欢迎来到胡萝卜塔防！\n\n"
        "敌人会沿道路从左向右前进，\n"
        "你的目标是阻止它们到达右边的萝卜。\n\n"
        "点击屏幕任意处继续");
    m_steps[0].showArrow = true;
    m_steps[0].arrowDir = ArrowDirection::Down;
    m_steps[0].arrowTargetX = m_grid->endX();
    m_steps[0].arrowTargetY = m_grid->endY();
    m_steps[0].completion = CompletionType::ClickContinue;

    // ── Step 1 ──
    m_steps[1].text = QString::fromUtf8(
        "这是暂停按钮。\n"
        "点击它可以随时暂停或继续游戏，\n"
        "也可以按键盘的 Esc 键。\n\n"
        "点击屏幕任意处继续");
    m_steps[1].showArrow = true;
    m_steps[1].arrowDir = ArrowDirection::Up;
    m_steps[1].completion = CompletionType::ClickContinue;

    // ── Step 2 ──
    m_steps[2].text = QString::fromUtf8(
        "这里显示了你的金币、生命值、\n"
        "当前波次和场上敌人数量。\n\n"
        "点击屏幕任意处继续");
    m_steps[2].showArrow = true;
    m_steps[2].arrowDir = ArrowDirection::Up;
    m_steps[2].completion = CompletionType::ClickContinue;

    // ── Step 3 ──
    m_steps[3].text = QString::fromUtf8(
        "点击箭头指向的格子来放置一座防御塔。\n"
        "试试看！");
    m_steps[3].showArrow = true;
    m_steps[3].arrowDir = ArrowDirection::Down;
    m_steps[3].arrowTargetX = 7;
    m_steps[3].arrowTargetY = 6;
    m_steps[3].completion = CompletionType::PlaceTower;
    m_steps[3].paramX = 7;
    m_steps[3].paramY = 6;

    // ── Step 4 ──
    m_steps[4].text = QString::fromUtf8(
        "敌人来了！观察你的塔如何自动攻击。");
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
    m_steps[5].completion = CompletionType::TowerUpgraded;
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
}

void TutorialController::start()
{
    m_active = true;
    m_currentStep = 0;
    m_killDetected = false;

    m_gc->setTimeScale(0);
    m_parentWidget->installEventFilter(this);
    connect(m_arrow, &TutorialArrow::clicked,
            this, &TutorialController::onArrowClicked);
    updateArrow();
}

bool TutorialController::eventFilter(QObject*, QEvent* event)
{
    if (!m_active) return false;

    // ── Resize ──
    if (event->type() == QEvent::Resize) {
        QTimer::singleShot(0, this, [this]() { updateArrow(); });
        return false;
    }

    // ── ClickCell: detect correct click from hole, let through ──
    if (event->type() == QEvent::MouseButtonPress) {
        auto& step = m_steps[m_currentStep];
        if (step.completion == CompletionType::ClickCell) {
            auto* me = static_cast<QMouseEvent*>(event);
            double gx = (me->pos().x() - m_grid->offsetX()) / m_grid->cellSize();
            double gy = (me->pos().y() - m_grid->offsetY()) / m_grid->cellSize();
            if (std::abs(gx - step.paramX) < 1.5
                && std::abs(gy - step.paramY) < 1.5) {
                QTimer::singleShot(0, this, &TutorialController::nextStep);
            }
        }
    }

    return false;  // let through to InputHandler
}

void TutorialController::onArrowClicked()
{
    if (!m_active) return;
    auto& step = m_steps[m_currentStep];
    if (step.completion == CompletionType::ClickContinue) {
        nextStep();
    }
}

void TutorialController::nextStep()
{
    if (!m_active) return;

    disconnect(m_stepConnection);

    m_currentStep++;
    if (m_currentStep >= 7) {
        m_active = false;
        m_parentWidget->removeEventFilter(this);
        m_arrow->clearHighlight();
        m_arrow->hide();
        m_gc->setTimeScale(1.0);
        return;
    }

    auto& step = m_steps[m_currentStep];

    // Step 4: resume game, wait for enemy kill
    if (step.completion == CompletionType::EnemyKilled) {
        m_arrow->clearHighlight();
        m_arrow->hide();
        m_killDetected = false;
        m_scene->resumeClock();
        m_gc->setTimeScale(1.0);
        return;
    }

    // All other steps: freeze and show UI
    m_gc->setTimeScale(0);
    updateArrow();

    // PlaceTower: connect towersChanged to auto-detect completion
    if (step.completion == CompletionType::PlaceTower) {
        m_stepConnection = connect(m_tm, &TowerManager::towersChanged,
            this, &TutorialController::checkAfterInput);
    }

    // TowerUpgraded: connect statsChanged to auto-detect upgrade
    if (step.completion == CompletionType::TowerUpgraded) {
        m_stepConnection = connect(m_gc, &GameController::statsChanged,
            this, &TutorialController::checkAfterInput);
    }
}

void TutorialController::checkAfterInput()
{
    if (!m_active) return;
    auto& step = m_steps[m_currentStep];

    if (step.completion == CompletionType::PlaceTower) {
        if (step.paramX >= 0 && step.paramY >= 0
            && m_tm->getTowerAt(step.paramX, step.paramY) != nullptr) {
            nextStep();
        }
    }

    if (step.completion == CompletionType::TowerUpgraded) {
        Tower* t = m_tm->getTowerAt(step.paramX, step.paramY);
        if (t && t->level() >= 2) {
            nextStep();
        }
    }
}

void TutorialController::updateArrow()
{
    if (m_currentStep < 0 || m_currentStep >= 7) return;
    auto& step = m_steps[m_currentStep];
    m_arrow->setGeometry(m_parentWidget->rect());

    // Interactive: cutout overlay + highlight + arrow + text
    if (step.completion == CompletionType::PlaceTower
        || step.completion == CompletionType::ClickCell
        || step.completion == CompletionType::TowerUpgraded) {
        if (step.arrowTargetX >= 0 && step.arrowTargetY >= 0) {
            double x = m_grid->offsetX()
                       + step.arrowTargetX * m_grid->cellSize();
            double y = m_grid->offsetY()
                       + step.arrowTargetY * m_grid->cellSize();
            m_arrow->setHighlightRect(QRectF(x + 1, y + 1,
                                              m_grid->cellSize() - 2,
                                              m_grid->cellSize() - 2));
        }
        m_arrow->setTarget(arrowFromPoint(), arrowTargetPoint(), step.arrowDir);
        m_arrow->setMessage(step.text);
        m_arrow->show();
        m_arrow->raise();
        return;
    }

    // EnemyKilled: hide everything
    if (step.completion == CompletionType::EnemyKilled) {
        m_arrow->hide();
        return;
    }

    // ClickContinue: dark overlay + message + optional arrow
    m_arrow->clearHighlight();
    m_arrow->setMessage(step.text);
    if (step.showArrow) {
        m_arrow->setTarget(arrowFromPoint(), arrowTargetPoint(), step.arrowDir);
    }
    m_arrow->show();
    m_arrow->raise();
}

QPoint TutorialController::arrowFromPoint() const
{
    QPoint target = arrowTargetPoint();
    int offset = 80;
    if (m_steps[m_currentStep].arrowDir == ArrowDirection::Up)
        return QPoint(target.x(), target.y() + offset);
    else
        return QPoint(target.x(), target.y() - offset);
}

QPoint TutorialController::arrowTargetPoint() const
{
    auto& step = m_steps[m_currentStep];

    // HUD targets
    if (step.arrowDir == ArrowDirection::Up) {
        if (m_currentStep == 1)
            return QPoint(50, 42);              // Pause 左固定 50px
        else
            return QPoint(m_parentWidget->width() * 0.55, 42);  // 状态栏靠右
    }

    // Map targets
    if (step.arrowTargetX >= 0 && step.arrowTargetY >= 0) {
        QPointF cell = m_grid->gridToPixel(step.arrowTargetX,
                                            step.arrowTargetY);
        return QPoint(static_cast<int>(cell.x()),
                      static_cast<int>(cell.y() - m_grid->cellSize() * 0.5));
    }

    return QPoint(m_parentWidget->width() / 2,
                  m_parentWidget->height() / 2);
}

void TutorialController::onEnemyKilled()
{
    if (!m_active) return;
    auto& step = m_steps[m_currentStep];
    if (step.completion == CompletionType::EnemyKilled && !m_killDetected) {
        m_killDetected = true;
        m_gc->setTimeScale(0);
        nextStep();
    }
}
