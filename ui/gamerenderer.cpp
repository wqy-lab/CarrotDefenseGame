#include "gamerenderer.h"
#include "../game/gamecontroller.h"
#include "../game/spatialgrid.h"
#include "../game/towermanager.h"
#include "../game/towers/tower.h"
#include "../game/towers/towerfactory.h"
#include "../game/towers/remotetower.h"
#include "../game/towers/meleetower.h"
#include "../game/cellentities.h"
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

    if (m_gameController && m_gameController->isGameOver()) {
        p.fillRect(rect(), QColor(0, 0, 0, 150));
    }
}

void GameRenderer::drawGrid(QPainter& p)
{
    if (!m_spatialGrid) return;

    double totalW = m_spatialGrid->gridCols() * m_spatialGrid->cellSize();
    double totalH = m_spatialGrid->gridRows() * m_spatialGrid->cellSize();

    p.fillRect(QRectF(m_spatialGrid->offsetX(), m_spatialGrid->offsetY(), totalW, totalH),
               QColor(60, 80, 45));

    for (int y = 0; y < m_spatialGrid->gridRows(); ++y) {
        for (int x = 0; x < m_spatialGrid->gridCols(); ++x) {
            if (m_spatialGrid->isPathCell(x, y)) {
                p.fillRect(QRectF(
                    m_spatialGrid->offsetX() + x * m_spatialGrid->cellSize() + 1,
                    m_spatialGrid->offsetY() + y * m_spatialGrid->cellSize() + 1,
                    m_spatialGrid->cellSize() - 2, m_spatialGrid->cellSize() - 2),
                    QColor(120, 90, 55));
            }
        }
    }

    if (m_towerManager) {
        for (auto& t : m_towerManager->towers()) {
            int x = t->gridX(), y = t->gridY();
            p.fillRect(QRectF(
                m_spatialGrid->offsetX() + x * m_spatialGrid->cellSize() + 1,
                m_spatialGrid->offsetY() + y * m_spatialGrid->cellSize() + 1,
                m_spatialGrid->cellSize() - 2, m_spatialGrid->cellSize() - 2),
                QColor(45, 50, 40));
        }
    }

    for (int y = 0; y < m_spatialGrid->gridRows(); ++y) {
        for (int x = 0; x < m_spatialGrid->gridCols(); ++x) {
            if (m_spatialGrid->isObstacleCell(x, y)) {
                p.fillRect(QRectF(
                    m_spatialGrid->offsetX() + x * m_spatialGrid->cellSize() + 1,
                    m_spatialGrid->offsetY() + y * m_spatialGrid->cellSize() + 1,
                    m_spatialGrid->cellSize() - 2, m_spatialGrid->cellSize() - 2),
                    QColor(90, 70, 50));
            }
        }
    }

    p.setPen(QPen(QColor(90, 100, 80), 1));
    for (int x = 0; x <= m_spatialGrid->gridCols(); ++x) {
        p.drawLine(QPointF(m_spatialGrid->offsetX() + x * m_spatialGrid->cellSize(), m_spatialGrid->offsetY()),
                    QPointF(m_spatialGrid->offsetX() + x * m_spatialGrid->cellSize(),
                            m_spatialGrid->offsetY() + m_spatialGrid->gridRows() * m_spatialGrid->cellSize()));
    }
    for (int y = 0; y <= m_spatialGrid->gridRows(); ++y) {
        p.drawLine(QPointF(m_spatialGrid->offsetX(), m_spatialGrid->offsetY() + y * m_spatialGrid->cellSize()),
                    QPointF(m_spatialGrid->offsetX() + m_spatialGrid->gridCols() * m_spatialGrid->cellSize(),
                            m_spatialGrid->offsetY() + y * m_spatialGrid->cellSize()));
    }

    {
        QPointF c = m_spatialGrid->gridToPixel(m_spatialGrid->startX(), m_spatialGrid->startY());
        double r = m_spatialGrid->cellSize() * 0.35;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(76, 175, 80));
        p.drawEllipse(c, r, r);
        p.setPen(Qt::white);
        QFont f("Arial", qMax(8, static_cast<int>(m_spatialGrid->cellSize() * 0.35)), QFont::Bold);
        p.setFont(f);
        p.drawText(QRectF(c.x()-r, c.y()-r, r*2, r*2), Qt::AlignCenter, "S");
    }

    {
        QPointF c = m_spatialGrid->gridToPixel(m_spatialGrid->endX(), m_spatialGrid->endY());
        double r = m_spatialGrid->cellSize() * 0.35;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(244, 67, 54));
        p.drawEllipse(c, r, r);
        p.setPen(Qt::white);
        QFont f("Arial", qMax(8, static_cast<int>(m_spatialGrid->cellSize() * 0.35)), QFont::Bold);
        p.setFont(f);
        p.drawText(QRectF(c.x()-r, c.y()-r, r*2, r*2), Qt::AlignCenter, "E");
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
        QPointF center = m_spatialGrid->gridToPixel(t->gridX(), t->gridY());
        double r = m_spatialGrid->cellSize() * 0.4;
        auto stats = t->stats();

        p.setPen(QPen(Qt::black, 2));
        p.setBrush(stats.color);
        p.drawRect(QRectF(center.x()-r, center.y()-r, r*2, r*2));

        p.setPen(Qt::white);
        QFont f("Arial", qMax(8, static_cast<int>(m_spatialGrid->cellSize()*0.3)), QFont::Bold);
        p.setFont(f);
        QString label;
        switch (t->type()) {
            case TowerType::Arrow: label = "A"; break;
            case TowerType::Cannon: label = "C"; break;
            case TowerType::Ice: label = "I"; break;
        }
        p.drawText(QRectF(center.x()-r, center.y()-r, r*2, r*2), Qt::AlignCenter, label);
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
    auto stats = previewTower->stats();
    QPointF center = m_spatialGrid->gridToPixel(hx, hy);
    double rangePx = stats.range * m_spatialGrid->cellSize();

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
