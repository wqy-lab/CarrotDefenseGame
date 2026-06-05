#include "gamerenderer.h"
#include "../game/gamecontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "../game/towers/tower.h"
#include "../game/towers/towerfactory.h"
#include "../game/towers/remotetower.h"
#include "../game/towers/meleetower.h"
#include "../game/cellentities.h"
#include "../game/config/datamanager.h"
#include "towerpanel.h"
#include <QPainter>

GameRenderer::GameRenderer(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void GameRenderer::render()
{
    update();
}

void GameRenderer::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.fillRect(rect(), QColor(34, 40, 34));

    drawGrid(p);
    drawPath(p);
    drawObstacles(p);
    drawTowers(p);
    drawEnemies(p);
    drawProjectiles(p);

    if (m_towerManager && m_towerManager->isPlacingTower() && m_towerManager->showRange()) {
        drawHoverPreview(p);
    }

    drawPriorityTarget(p);

    if (m_gameController && m_gameController->isGameOver()) {
        p.fillRect(rect(), QColor(0, 0, 0, 150));
    }
}

void GameRenderer::drawGrid(QPainter& p)
{
    if (!m_spatialGrid) return;

    double cs = m_spatialGrid->cellSize();
    double ox = m_spatialGrid->offsetX();
    double oy = m_spatialGrid->offsetY();
    int cols = m_spatialGrid->gridCols();
    int rows = m_spatialGrid->gridRows();

    double totalW = cols * cs;
    double totalH = rows * cs;

    const QPixmap& grassTex = DataManager::instance().getTexture("assets/tiles/grass.png");
    const QPixmap& pathTex = DataManager::instance().getTexture("assets/tiles/path.png");

    p.fillRect(QRectF(ox, oy, totalW, totalH), QColor(60, 80, 45));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            QRectF target(ox + x * cs, oy + y * cs, cs, cs);
            if (m_spatialGrid->isPathCell(x, y)) {
                if (!pathTex.isNull()) {
                    p.drawPixmap(target.toRect(), pathTex);
                } else {
                    p.fillRect(target, QColor(120, 90, 55));
                }
            } else if (!m_spatialGrid->isObstacleCell(x, y)) {
                if (!grassTex.isNull()) {
                    p.drawPixmap(target.toRect(), grassTex);
                }
            }
        }
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (m_spatialGrid->isObstacleCell(x, y)) {
                p.fillRect(QRectF(ox + x * cs, oy + y * cs, cs, cs),
                           QColor(90, 70, 50));
            }
        }
    }

    p.setPen(QPen(QColor(90, 100, 80, 30), 1));
    for (int x = 0; x <= cols; ++x) {
        p.drawLine(QPointF(ox + x * cs, oy), QPointF(ox + x * cs, oy + rows * cs));
    }
    for (int y = 0; y <= rows; ++y) {
        p.drawLine(QPointF(ox, oy + y * cs), QPointF(ox + cols * cs, oy + y * cs));
    }

    const QPixmap& startTex = DataManager::instance().getTexture("assets/tiles/start.png");
    const QPixmap& endTex = DataManager::instance().getTexture("assets/base/carrot.png");

    {
        QPointF c = m_spatialGrid->gridToPixel(m_spatialGrid->startX(), m_spatialGrid->startY());
        double r = cs * 0.42;
        if (!startTex.isNull()) {
            QRectF target(c.x() - r, c.y() - r, r * 2, r * 2);
            p.drawPixmap(target.toRect(), startTex);
        } else {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(76, 175, 80));
            p.drawEllipse(c, r, r);
            p.setPen(Qt::white);
            QFont f("Arial", qMax(8, static_cast<int>(cs * 0.35)), QFont::Bold);
            p.setFont(f);
            p.drawText(QRectF(c.x()-r, c.y()-r, r*2, r*2), Qt::AlignCenter, "S");
        }
    }

    {
        QPointF c = m_spatialGrid->gridToPixel(m_spatialGrid->endX(), m_spatialGrid->endY());
        double r = cs * 0.42;
        if (!endTex.isNull()) {
            QRectF target(c.x() - r, c.y() - r, r * 2, r * 2);
            p.drawPixmap(target.toRect(), endTex);
        } else {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(244, 67, 54));
            p.drawEllipse(c, r, r);
            p.setPen(Qt::white);
            QFont f("Arial", qMax(8, static_cast<int>(cs * 0.35)), QFont::Bold);
            p.setFont(f);
            p.drawText(QRectF(c.x()-r, c.y()-r, r*2, r*2), Qt::AlignCenter, "E");
        }
    }
}

void GameRenderer::drawPath(QPainter& p)
{
    if (!m_spatialGrid) return;
    const auto& waypoints = m_spatialGrid->waypoints();
    if (waypoints.size() < 2) return;

    p.setPen(QPen(QColor(200, 170, 120, 100), m_spatialGrid->cellSize() * 0.15, Qt::SolidLine, Qt::RoundCap));
    for (size_t i = 1; i < waypoints.size(); ++i) {
        QPointF prev = m_spatialGrid->gridToPixel(static_cast<int>(waypoints[i-1].x()), static_cast<int>(waypoints[i-1].y()));
        QPointF curr = m_spatialGrid->gridToPixel(static_cast<int>(waypoints[i].x()), static_cast<int>(waypoints[i].y()));
        p.drawLine(prev, curr);
    }
}

void GameRenderer::drawTowers(QPainter& p)
{
    if (!m_towerManager) return;

    for (auto& t : m_towerManager->towers()) {
        t->draw(p, m_spatialGrid->cellSize(), m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
    }
}

void GameRenderer::drawEnemies(QPainter& p)
{
    if (!m_gameController) return;
    for (auto& e : m_gameController->enemies()) {
        if (e->isActive()) e->draw(p, m_spatialGrid->cellSize(), m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
    }
}

void GameRenderer::drawProjectiles(QPainter& p)
{
    if (!m_gameController) return;
    for (auto& pj : m_gameController->projectiles()) {
        if (pj->isActive()) pj->draw(p, m_spatialGrid->cellSize(), m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
    }
}

void GameRenderer::drawObstacles(QPainter& p)
{
    if (!m_gameController) return;
    for (auto& obs : m_gameController->obstacles()) {
        obs->draw(&p, m_spatialGrid->cellSize(), m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
    }
}

void GameRenderer::drawHoverPreview(QPainter& p)
{
    if (!m_towerManager || !m_spatialGrid) return;

    int hx = m_towerManager->hoverGridX();
    int hy = m_towerManager->hoverGridY();

    if (!m_spatialGrid->isValidGridPos(hx, hy)) return;
    if (m_spatialGrid->isPathCell(hx, hy)) return;
    if (m_spatialGrid->isObstacleCell(hx, hy)) return;
    if (m_towerManager->getTowerAt(hx, hy)) return;

    auto previewTower = createTower(m_towerManager->selectedTower(), hx, hy,
                                   m_spatialGrid->cellSize(),
                                   m_spatialGrid->offsetX(),
                                   m_spatialGrid->offsetY());
    QPointF center = m_spatialGrid->gridToPixel(hx, hy);
    double rangePx = previewTower->range() * m_spatialGrid->cellSize();

    p.setPen(QPen(QColor(255, 255, 255, 80), 1, Qt::DashLine));
    p.setBrush(QColor(100, 200, 100, 30));
    p.drawEllipse(center, rangePx, rangePx);

    p.setPen(QPen(QColor(100, 200, 100), 2));
    p.setBrush(QColor(100, 200, 100, 40));
    p.drawRect(QRectF(
        m_spatialGrid->offsetX() + hx * m_spatialGrid->cellSize() + 1,
        m_spatialGrid->offsetY() + hy * m_spatialGrid->cellSize() + 1,
        m_spatialGrid->cellSize() - 2, m_spatialGrid->cellSize() - 2));
}

void GameRenderer::drawPriorityTarget(QPainter& p)
{
    if (!m_gameController || !m_spatialGrid) return;

    double cs = m_spatialGrid->cellSize();
    double ox = m_spatialGrid->offsetX();
    double oy = m_spatialGrid->offsetY();

    QPointF center;
    int radius = 12;

    Enemy* e = m_gameController->priorityEnemy();
    if (e && e->isActive()) {
        center = e->pos(cs, ox, oy);
        radius = e->radius();
    } else {
        Obstacle* obs = m_gameController->priorityObstacle();
        if (obs && obs->isActive()) {
            double cx = ox + (obs->gridX() + obs->gridWidth() / 2.0) * cs;
            double cy = oy + (obs->gridY() + obs->gridHeight() / 2.0) * cs;
            center = QPointF(cx, cy);
            radius = obs->radius();
        } else {
            return;
        }
    }

    const QPixmap& tex = DataManager::instance().getTexture("assets/ui/target_lock.png");
    if (!tex.isNull()) {
        int hw = tex.width() / 2;
        int hh = tex.height() / 2;
        p.drawPixmap(static_cast<int>(center.x()) - hw,
                     static_cast<int>(center.y()) - hh, tex);
        return;
    }

    int len = qMin(radius + 6, 16);
    p.setPen(QPen(QColor(255, 50, 50), 2));
    p.drawLine(QPointF(center.x() - len, center.y()),
               QPointF(center.x() + len, center.y()));
    p.drawLine(QPointF(center.x(), center.y() - len),
               QPointF(center.x(), center.y() + len));
}
