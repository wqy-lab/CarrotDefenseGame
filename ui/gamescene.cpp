#include "gamescene.h"
#include "../game/enemies/enemyfactory.h"
#include "../game/towers/towerfactory.h"
#include "../game/bullets/bulletfactory.h"
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

    initPath();

    connect(m_gameTimer, &QTimer::timeout, this, &GameScene::gameLoop);
}

void GameScene::initPath()
{
    // Clear
    m_isPath.assign(GRID_ROWS, std::vector<bool>(GRID_COLS, false));
    m_pathSet.clear();
    m_waypoints.clear();

    // Define winding path as grid coords (snake through map)
    // Segment 1: right along row 6
    for (int x = 0; x <= 3; x++)  { m_pathSet.insert({x, 6}); }
    // Segment 2: up along col 3 to row 2
    for (int y = 6; y >= 2; y--)  { m_pathSet.insert({3, y}); }
    // Segment 3: right along row 2
    for (int x = 4; x <= 6; x++)  { m_pathSet.insert({x, 2}); }
    // Segment 4: down along col 6 to row 9
    for (int y = 3; y <= 9; y++)  { m_pathSet.insert({6, y}); }
    // Segment 5: right along row 9
    for (int x = 7; x <= 10; x++) { m_pathSet.insert({x, 9}); }
    // Segment 6: up along col 10 to row 4
    for (int y = 9; y >= 4; y--)  { m_pathSet.insert({10, y}); }
    // Segment 7: right along row 4
    for (int x = 11; x <= 14; x++) { m_pathSet.insert({x, 4}); }
    // Segment 8: down to end at (14,6)
    for (int y = 5; y <= 6; y++)  { m_pathSet.insert({14, y}); }

    // Mark grid
    for (auto& p : m_pathSet) {
        m_isPath[p.second][p.first] = true;
    }

    // Pre-compute pixel waypoints for enemies (ordered path traversal)
    // Rebuild in order:
    for (int x = 0; x <= 3; x++)  m_waypoints.push_back(gridToPixel(x, 6));
    for (int y = 5; y >= 2; y--)  m_waypoints.push_back(gridToPixel(3, y));
    for (int x = 4; x <= 6; x++)  m_waypoints.push_back(gridToPixel(x, 2));
    for (int y = 3; y <= 9; y++)  m_waypoints.push_back(gridToPixel(6, y));
    for (int x = 7; x <= 10; x++) m_waypoints.push_back(gridToPixel(x, 9));
    for (int y = 8; y >= 4; y--)  m_waypoints.push_back(gridToPixel(10, y));
    for (int x = 11; x <= 14; x++) m_waypoints.push_back(gridToPixel(x, 4));
    for (int y = 5; y <= 6; y++)  m_waypoints.push_back(gridToPixel(14, y));
}

// ============ Game State ============

void GameScene::startGame()
{
    resetGame();
    m_gameRunning = true;
    m_paused = false;
    m_waveManager.nextWave();
    m_gameTimer->start(16);
}

void GameScene::pauseGame()  { m_paused = true; }
void GameScene::resumeGame() { m_paused = false; }

void GameScene::resetGame()
{
    m_gameTimer->stop();
    m_gameRunning = false;
    m_paused = false;
    m_gameOver = false;
    m_victory = false;
    m_gold = 200;
    m_lives = 10;

    m_towers.clear();
    m_enemies.clear();
    m_projectiles.clear();
    m_waveManager.reset();

    // Restore path (in case towers removed path cells)
    initPath();

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
    double dt = 16.0 / 1000.0;
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
                m_projectiles.push_back(std::move(b));
            }
        }
    }

    // Update projectiles
    for (auto& p : m_projectiles) {
        if (p->isActive()) p->update(dt, m_enemies);
    }

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
            m_gold += 30 + m_waveManager.currentWave() * 5;
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
    return gx >= 0 && gx < GRID_COLS && gy >= 0 && gy < GRID_ROWS;
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
    if (m_placingTower && isValidGridPos(m_hoverGridX, m_hoverGridY) && !isPathCell(m_hoverGridX, m_hoverGridY)) {
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
    if (isValidGridPos(g.x(), g.y()) && !isPathCell(g.x(), g.y())) {
        placeTower(g.x(), g.y());
    }
}

// ============ Resize ============

void GameScene::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    double w = static_cast<double>(width());
    double h = static_cast<double>(height());
    m_cellSize = qMin(w / GRID_COLS, h / GRID_ROWS);
    if (m_cellSize < 20) m_cellSize = 20;
    m_offsetX = (w - GRID_COLS * m_cellSize) / 2.0;
    m_offsetY = (h - GRID_ROWS * m_cellSize) / 2.0;

    // Rebuild pixel waypoints on resize
    m_waypoints.clear();
    for (int x = 0; x <= 3; x++)  m_waypoints.push_back(gridToPixel(x, 6));
    for (int y = 5; y >= 2; y--)  m_waypoints.push_back(gridToPixel(3, y));
    for (int x = 4; x <= 6; x++)  m_waypoints.push_back(gridToPixel(x, 2));
    for (int y = 3; y <= 9; y++)  m_waypoints.push_back(gridToPixel(6, y));
    for (int x = 7; x <= 10; x++) m_waypoints.push_back(gridToPixel(x, 9));
    for (int y = 8; y >= 4; y--)  m_waypoints.push_back(gridToPixel(10, y));
    for (int x = 11; x <= 14; x++) m_waypoints.push_back(gridToPixel(x, 4));
    for (int y = 5; y <= 6; y++)  m_waypoints.push_back(gridToPixel(14, y));

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
    double totalW = GRID_COLS * m_cellSize;
    double totalH = GRID_ROWS * m_cellSize;

    // Base fill
    p.fillRect(QRectF(m_offsetX, m_offsetY, totalW, totalH), QColor(60, 80, 45));

    // Path cells: darker road color
    for (int y = 0; y < GRID_ROWS; ++y) {
        for (int x = 0; x < GRID_COLS; ++x) {
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

    // Grid lines
    p.setPen(QPen(QColor(90, 100, 80), 1));
    for (int x = 0; x <= GRID_COLS; ++x)
        p.drawLine(QPointF(m_offsetX + x * m_cellSize, m_offsetY),
                    QPointF(m_offsetX + x * m_cellSize, m_offsetY + totalH));
    for (int y = 0; y <= GRID_ROWS; ++y)
        p.drawLine(QPointF(m_offsetX, m_offsetY + y * m_cellSize),
                    QPointF(m_offsetX + totalW, m_offsetY + y * m_cellSize));

    // Start marker (S)
    {
        QPointF c = gridToPixel(START_X, START_Y);
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
        QPointF c = gridToPixel(END_X, END_Y);
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
