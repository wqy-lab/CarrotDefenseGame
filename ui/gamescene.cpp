#include "gamescene.h"
#include "../game/enemies/enemyfactory.h"
#include "../game/towers/towerfactory.h"
#include "../game/bullets/bulletfactory.h"
#include "../game/config/datamanager.h"
#include "../game/obstacles/obstaclefactory.h"
#include <QPainterPath>
#include <QtMath>
#include <algorithm>
#include <chrono>
#include <random>

GameScene::GameScene(QWidget* parent)
    : QWidget(parent)
    , m_cellSize(48)
    , m_offsetX(0), m_offsetY(0)
    , m_gameRunning(false)
    , m_paused(false)
    , m_gameOver(false)
    , m_victory(false)
    , m_gold(200)
    , m_lives(10)
    , m_selectedTower(TowerType::Arrow)
    , m_placingTower(false)
    , m_hoverGridX(-1), m_hoverGridY(-1)
    , m_showRange(false)
    , m_gameTimer(new QTimer(this))
{
    setMouseTracking(true);
    setMinimumSize(600, 450);
    setFocusPolicy(Qt::StrongFocus);

    initMap(DataManager::instance().mapData());

    connect(m_gameTimer, &QTimer::timeout, this, &GameScene::gameLoop);
}

void GameScene::initMap(const MapData& map)
{
    m_gridCols = map.gridCols;
    m_gridRows = map.gridRows;
    m_startX   = map.startX;
    m_startY   = map.startY;
    m_endX     = map.endX;
    m_endY     = map.endY;

    m_isPath.assign(m_gridRows, std::vector<bool>(m_gridCols, false));
    m_obstacleCell.assign(m_gridRows, std::vector<bool>(m_gridCols, false));
    m_entityGrid.assign(m_gridRows, std::vector<CellEntities>(m_gridCols));
    m_pathSet.clear();
    m_waypoints.clear();
    m_obstacles.clear();

    for (const QPoint& p : map.pathCells)
    {
        m_pathSet.insert({p.x(), p.y()});
        m_isPath[p.y()][p.x()] = true;
        m_waypoints.push_back(gridToPixel(p.x(), p.y()));
    }

    // Create obstacles from DataManager
    for (const auto& [obsType, gridPos] : DataManager::instance().obstacles()) {
        int gx = gridPos.x(), gy = gridPos.y();
        if (isValidGridPos(gx, gy) && !isPathCell(gx, gy)) {
            m_obstacleCell[gy][gx] = true;
            QPointF pixelPos = gridToPixel(gx, gy);
            m_obstacles.push_back(createObstacle(obsType, gx, gy, pixelPos));
        }
    }
}

// ============ Game State ============

void GameScene::startGame()
{
    resetGame();
    m_gameRunning = true;
    m_paused = false;
    m_clock.start();
    m_waveManager.nextWave();
    m_gameTimer->start(16);
}

void GameScene::pauseGame()  { m_paused = true; }
void GameScene::resumeGame()
{
    m_clock.restart();
    m_paused = false;
}

void GameScene::resetGame()
{
    m_gameTimer->stop();
    m_gameRunning = false;
    m_paused = false;
    m_gameOver = false;
    m_victory = false;
    m_gold = DataManager::instance().initialGold();
    m_lives = DataManager::instance().initialLives();

    m_towers.clear();
    m_enemies.clear();
    m_projectiles.clear();
    m_obstacles.clear();
    m_waveManager.reset();

    // Restore path (in case towers removed path cells)
    initMap(DataManager::instance().mapData());

    update();
}

void GameScene::selectTowerType(TowerType type)
{
    m_selectedTower = type;
    m_placingTower = true;
    m_showRange = true;
    update();
}

// ============ Game Loop ============

void GameScene::gameLoop()
{
    if (!m_gameRunning || m_paused || m_gameOver) return;

    double dt = m_clock.restart() / 1000.0;
    if (dt > 0.1) dt = 0.1;  // clamp to 100ms max (prevents spiral after hang)

    updateGame(dt);
    update();
}

void GameScene::updateGame(double dt)
{
    // Spawn enemies
    m_waveManager.update(dt);
    while (m_waveManager.shouldSpawn()) {
        spawnEnemy();
    }

    // Update enemies
    for (auto& e : m_enemies) {
        if (e->isActive()) e->update(dt);
    }

    // Process enemies that reached end
    for (auto& e : m_enemies) {
        if (e->reachedEnd()) {
            m_lives -= e->damage();
            if (m_lives < 0) m_lives = 0;
        }
    }

    // Update towers
    for (auto& t : m_towers) {
        t->update(dt, m_enemies);
    }

    // Create projectiles from tower attacks
    for (auto& t : m_towers) {
        if (MeleeTower* mt = dynamic_cast<MeleeTower*>(t.get())) {
            if (mt->hasPendingEffect()) {
                auto effect = mt->getEffect();
                for (auto& e : m_enemies) {
                    if (!e->isActive()) continue;
                    QPointF d = e->pos() - effect.center;
                    double dist = std::sqrt(d.x()*d.x() + d.y()*d.y());
                    if (dist <= effect.radius) {
                        double falloff = 1.0 - (dist / effect.radius) * 0.5;
                        e->takeDamage(effect.damage * falloff);
                        if (effect.slowFactor < 1.0)
                            e->applySlow(effect.slowFactor, effect.slowDuration);
                        if (effect.poisonDps > 0)
                            e->applyPoison(effect.poisonDps, effect.poisonDuration);
                    }
                }
            }
        } else if (RemoteTower* rt = dynamic_cast<RemoteTower*>(t.get())) {
            if (rt->hasPendingAttack()) {
                auto attack = rt->getAttack();
                auto b = createBullet(BulletType::Normal, t->centerPos(), attack.targetPos,
                                      attack.damage, attack.splashRadius, attack.slowFactor,
                                      attack.slowDuration, attack.poisonDps, attack.poisonDuration,
                                      attack.chainCount, attack.color);
                b->setMaxDistance(attack.maxDistance);
                m_projectiles.push_back(std::move(b));
            }
        }
    }

    // Update projectiles
    syncEntityGrid();
    for (auto& p : m_projectiles) {
        if (p->isActive()) {
            QPointF pos = p->pos();
            QPoint g = pixelToGrid(pos);
            CellEntities& cell = getCellAt(g.x(), g.y());
            p->update(dt, cell);
        }
    }

    // Update obstacles
    updateObstacles(dt);

    // Process hits
    for (auto& p : m_projectiles) {
        if (p->hasHit()) {
            handleProjectileHit(*p);
        }
    }

    // Remove dead/inactive
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](auto& e) { return e->isDead() || e->reachedEnd(); }),
        m_enemies.end());
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](auto& p) { return !p->isActive(); }),
        m_projectiles.end());

    // Wave completion
    if (m_waveManager.waveComplete() && m_enemies.empty()) {
        if (m_waveManager.allWavesDone()) {
            m_victory = true;
            m_gameOver = true;
            m_gameTimer->stop();
            emit gameEnded(true);
        } else {
            m_gold += DataManager::instance().waveBonusBase()
                      + m_waveManager.currentWave()
                      * DataManager::instance().waveBonusPerWave();
            m_waveManager.nextWave();
        }
    }

    checkGameEnd();
    emit statsChanged();
}

void GameScene::spawnEnemy()
{
    EnemyType type = m_waveManager.popSpawnType();
    m_enemies.push_back(createEnemy(type, m_waypoints));
}

void GameScene::handleProjectileHit(Bullet& proj)
{
    Q_UNUSED(proj);

    for (auto& e : m_enemies) {
        if (e->isDead() && e->reward() > 0) {
            if (e->consumeReward()) {
                m_gold += e->reward();
            }
        }
    }
}

void GameScene::checkGameEnd()
{
    if (m_lives <= 0) {
        m_lives = 0;
        m_gameOver = true;
        m_gameTimer->stop();
        emit gameEnded(false);
    }
}

// ============ Tower Placement ============

void GameScene::placeTower(int gx, int gy)
{
    if (!isValidGridPos(gx, gy)) return;
    if (isPathCell(gx, gy)) return;
    if (isObstacleCell(gx, gy)) return;

    // Check no tower already here
    for (auto& t : m_towers) {
        if (t->gridX() == gx && t->gridY() == gy) return;
    }

    auto t = createTower(m_selectedTower, gx, gy, m_cellSize, m_offsetX, m_offsetY);
    if (m_gold < t->cost()) return;

    m_gold -= t->cost();
    m_towers.push_back(std::move(t));

    update();
}

// ============ Coordinates ============

QPointF GameScene::gridToPixel(int gx, int gy) const
{
    return QPointF(
        m_offsetX + gx * m_cellSize + m_cellSize / 2.0,
        m_offsetY + gy * m_cellSize + m_cellSize / 2.0);
}

QPoint GameScene::pixelToGrid(const QPointF& pos) const
{
    return QPoint(
        static_cast<int>((pos.x() - m_offsetX) / m_cellSize),
        static_cast<int>((pos.y() - m_offsetY) / m_cellSize));
}

bool GameScene::isValidGridPos(int gx, int gy) const
{
    return gx >= 0 && gx < m_gridCols && gy >= 0 && gy < m_gridRows;
}

bool GameScene::isPathCell(int gx, int gy) const
{
    if (!isValidGridPos(gx, gy)) return true; // treat out of bounds as blocked
    return m_isPath[gy][gx];
}

// ============ Mouse ============

void GameScene::mouseMoveEvent(QMouseEvent* event)
{
    QPoint g = pixelToGrid(event->pos());
    m_hoverGridX = g.x();
    m_hoverGridY = g.y();
    if (m_placingTower && isValidGridPos(m_hoverGridX, m_hoverGridY) && !isPathCell(m_hoverGridX, m_hoverGridY) && !isObstacleCell(m_hoverGridX, m_hoverGridY)) {
        m_showRange = true;
    } else {
        m_showRange = false;
    }
    update();
}

void GameScene::mousePressEvent(QMouseEvent* event)
{
    if (!m_gameRunning || m_paused || m_gameOver) return;
    if (!m_placingTower) return;
    if (event->button() != Qt::LeftButton) return;

    QPoint g = pixelToGrid(event->pos());
    if (isValidGridPos(g.x(), g.y()) && !isPathCell(g.x(), g.y()) && !isObstacleCell(g.x(), g.y())) {
        placeTower(g.x(), g.y());
    }
}

// ============ Resize ============

void GameScene::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    double w = static_cast<double>(width());
    double h = static_cast<double>(height());
    m_cellSize = qMin(w / m_gridCols, h / m_gridRows);
    if (m_cellSize < 20) m_cellSize = 20;
    m_offsetX = (w - m_gridCols * m_cellSize) / 2.0;
    m_offsetY = (h - m_gridRows * m_cellSize) / 2.0;

    // Rebuild pixel waypoints on resize
    m_waypoints.clear();
    for (const QPoint& p : DataManager::instance().mapData().pathCells)
    {
        m_waypoints.push_back(gridToPixel(p.x(), p.y()));
    }

    // Update existing enemy paths
    for (auto& e : m_enemies) {
        if (e->isActive()) e->updatePath(m_waypoints);
    }

    update();
}

// ============ Rendering ============

void GameScene::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Background
    p.fillRect(rect(), QColor(34, 40, 34));

    drawGrid(p);
    drawPath(p);
    drawObstacles(p);
    drawTowers(p);
    drawEnemies(p);
    drawProjectiles(p);

    // Hover range
    if (m_placingTower && m_showRange) {
        auto previewTower = createTower(m_selectedTower, m_hoverGridX, m_hoverGridY, m_cellSize, m_offsetX, m_offsetY);
        TowerStats stats = previewTower->stats();
        QPointF center = gridToPixel(m_hoverGridX, m_hoverGridY);
        double rangePx = stats.range * m_cellSize;

        p.setPen(QPen(QColor(255, 255, 255, 80), 1, Qt::DashLine));
        p.setBrush(QColor(100, 200, 100, 30));
        p.drawEllipse(center, rangePx, rangePx);

        p.setPen(QPen(QColor(100, 200, 100), 2));
        p.setBrush(QColor(100, 200, 100, 40));
        p.drawRect(QRectF(
            m_offsetX + m_hoverGridX * m_cellSize + 1,
            m_offsetY + m_hoverGridY * m_cellSize + 1,
            m_cellSize - 2, m_cellSize - 2));
    }

    if (m_gameOver) {
        p.fillRect(rect(), QColor(0, 0, 0, 150));
        p.setPen(Qt::white);
        QFont f("Arial", 28, QFont::Bold);
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, m_victory ? "VICTORY!" : "GAME OVER");
    }
}

void GameScene::drawGrid(QPainter& p)
{
    double totalW = m_gridCols * m_cellSize;
    double totalH = m_gridRows * m_cellSize;

    // Base fill
    p.fillRect(QRectF(m_offsetX, m_offsetY, totalW, totalH), QColor(60, 80, 45));

    // Path cells: darker road color
    for (int y = 0; y < m_gridRows; ++y) {
        for (int x = 0; x < m_gridCols; ++x) {
            if (m_isPath[y][x]) {
                p.fillRect(QRectF(
                    m_offsetX + x * m_cellSize + 1,
                    m_offsetY + y * m_cellSize + 1,
                    m_cellSize - 2, m_cellSize - 2),
                    QColor(120, 90, 55)); // Road brown
            }
        }
    }

    // Tower cells: slightly darker
    for (auto& t : m_towers) {
        int x = t->gridX(), y = t->gridY();
        p.fillRect(QRectF(
            m_offsetX + x * m_cellSize + 1,
            m_offsetY + y * m_cellSize + 1,
            m_cellSize - 2, m_cellSize - 2),
            QColor(45, 50, 40));
    }

    // Obstacle cells: distinct color
    for (int y = 0; y < m_gridRows; ++y) {
        for (int x = 0; x < m_gridCols; ++x) {
            if (m_obstacleCell[y][x]) {
                p.fillRect(QRectF(
                    m_offsetX + x * m_cellSize + 1,
                    m_offsetY + y * m_cellSize + 1,
                    m_cellSize - 2, m_cellSize - 2),
                    QColor(90, 70, 50));
            }
        }
    }

    // Grid lines
    p.setPen(QPen(QColor(90, 100, 80), 1));
    for (int x = 0; x <= m_gridCols; ++x)
        p.drawLine(QPointF(m_offsetX + x * m_cellSize, m_offsetY),
                    QPointF(m_offsetX + x * m_cellSize, m_offsetY + totalH));
    for (int y = 0; y <= m_gridRows; ++y)
        p.drawLine(QPointF(m_offsetX, m_offsetY + y * m_cellSize),
                    QPointF(m_offsetX + totalW, m_offsetY + y * m_cellSize));

    // Start marker (S)
    {
        QPointF c = gridToPixel(m_startX, m_startY);
        double r = m_cellSize * 0.35;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(76, 175, 80));
        p.drawEllipse(c, r, r);
        p.setPen(Qt::white);
        QFont f("Arial", qMax(8, static_cast<int>(m_cellSize * 0.35)), QFont::Bold);
        p.setFont(f);
        p.drawText(QRectF(c.x()-r, c.y()-r, r*2, r*2), Qt::AlignCenter, "S");
    }

    // End marker (E)
    {
        QPointF c = gridToPixel(m_endX, m_endY);
        double r = m_cellSize * 0.35;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(244, 67, 54));
        p.drawEllipse(c, r, r);
        p.setPen(Qt::white);
        QFont f("Arial", qMax(8, static_cast<int>(m_cellSize * 0.35)), QFont::Bold);
        p.setFont(f);
        p.drawText(QRectF(c.x()-r, c.y()-r, r*2, r*2), Qt::AlignCenter, "E");
    }
}

void GameScene::drawPath(QPainter& p)
{
    if (m_waypoints.size() < 2) return;
    // Draw a subtle road center line (not a thick filled path)
    p.setPen(QPen(QColor(200, 170, 120, 100), m_cellSize * 0.15, Qt::SolidLine, Qt::RoundCap));
    for (size_t i = 1; i < m_waypoints.size(); ++i)
        p.drawLine(m_waypoints[i-1], m_waypoints[i]);
}

void GameScene::drawTowers(QPainter& p)
{
    for (auto& t : m_towers) {
        QPointF center = gridToPixel(t->gridX(), t->gridY());
        double r = m_cellSize * 0.4;
        TowerStats stats = t->stats();

        p.setPen(QPen(Qt::black, 2));
        p.setBrush(stats.color);
        p.drawRect(QRectF(center.x()-r, center.y()-r, r*2, r*2));

        p.setPen(Qt::white);
        QFont f("Arial", qMax(8, static_cast<int>(m_cellSize*0.3)), QFont::Bold);
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

void GameScene::drawEnemies(QPainter& p)
{
    for (auto& e : m_enemies)
        if (e->isActive()) e->draw(p);
}

void GameScene::drawProjectiles(QPainter& p)
{
    for (auto& pj : m_projectiles)
        if (pj->isActive()) pj->draw(p);
}

void GameScene::drawObstacles(QPainter& p)
{
    for (auto& obs : m_obstacles)
        obs->draw(&p);
}

void GameScene::updateObstacles(double dt)
{
    for (auto& obs : m_obstacles) {
        obs->update(dt);
    }

    for (auto& obs : m_obstacles) {
        if (obs->isDestroyed()) {
            m_gold += obs->reward();
            m_obstacleCell[obs->gridY()][obs->gridX()] = false;
        }
    }

    m_obstacles.erase(
        std::remove_if(m_obstacles.begin(), m_obstacles.end(),
            [](auto& obs) { return obs->isDestroyed(); }),
        m_obstacles.end());
}

void GameScene::syncEntityGrid()
{
    for (auto& row : m_entityGrid) {
        for (auto& cell : row) {
            cell.enemies.clear();
            cell.obstacles.clear();
        }
    }

    for (auto& e : m_enemies) {
        if (!e->isActive()) continue;
        QPoint g = pixelToGrid(e->pos());
        if (isValidGridPos(g.x(), g.y())) {
            m_entityGrid[g.y()][g.x()].enemies.push_back(e.get());
        }
    }

    for (auto& obs : m_obstacles) {
        if (!obs->isActive()) continue;
        int gx = obs->gridX(), gy = obs->gridY();
        if (isValidGridPos(gx, gy)) {
            m_entityGrid[gy][gx].obstacles.push_back(obs.get());
        }
    }
}

CellEntities& GameScene::getCellAt(int gx, int gy)
{
    static CellEntities dummy;
    if (!isValidGridPos(gx, gy)) return dummy;
    return m_entityGrid[gy][gx];
}

bool GameScene::isObstacleCell(int gx, int gy) const
{
    if (!isValidGridPos(gx, gy)) return false;
    return m_obstacleCell[gy][gx];
}
