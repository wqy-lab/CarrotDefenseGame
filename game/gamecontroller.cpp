#include "gamecontroller.h"
#include "spatialgrid.h"
#include "enemies/enemyfactory.h"
#include "bullets/bulletfactory.h"
#include "obstacles/obstaclefactory.h"
#include "wave.h"
#include "config/datamanager.h"
#include "towers/tower.h"
#include "towers/remotetower.h"
#include "towers/meleetower.h"
#include <algorithm>
#include <cmath>

GameController::GameController(SpatialGrid* spatialGrid, QObject* parent)
    : QObject(parent)
    , m_spatialGrid(spatialGrid)
{
}

void GameController::startGame()
{
    resetGame();
    m_gameRunning = true;
    m_paused = false;
    m_waveManager.nextWave();
}

void GameController::pauseGame()
{
    m_paused = true;
}

void GameController::resumeGame()
{
    m_paused = false;
}

void GameController::resetGame()
{
    m_gameRunning = false;
    m_paused = false;
    m_gameOver = false;
    m_victory = false;
    m_gold = DataManager::instance().initialGold();
    m_lives = DataManager::instance().initialLives();

    m_enemies.clear();
    m_projectiles.clear();
    m_obstacles.clear();
    m_waveManager.reset();

    if (m_spatialGrid) {
        m_spatialGrid->initMap(DataManager::instance().mapData());
    }

    // Create obstacles from DataManager
    for (const auto& entry : DataManager::instance().obstacles()) {
        int gx = entry.gridX, gy = entry.gridY;
        int gw = entry.gridW, gh = entry.gridH;

        // Mark all occupied cells in spatial grid
        for (int dy = 0; dy < gh; ++dy) {
            for (int dx = 0; dx < gw; ++dx) {
                int cx = gx + dx, cy = gy + dy;
                if (m_spatialGrid->isValidGridPos(cx, cy) && !m_spatialGrid->isPathCell(cx, cy)) {
                    // Note: obstacleCell marking should be done separately if needed
                }
            }
        }

        auto obs = createObstacle(entry.type, gx, gy, gw, gh);
        m_obstacles.push_back(std::move(obs));
    }
}

void GameController::reduceLives(int amount)
{
    m_lives -= amount;
    if (m_lives < 0) m_lives = 0;
}

void GameController::update(double dt, const std::vector<std::unique_ptr<Tower>>& towers)
{
    if (!m_gameRunning || m_paused || m_gameOver) return;

    if (dt > 0.1) dt = 0.1;
    updateGame(dt, towers);
}

void GameController::updateGame(double dt, const std::vector<std::unique_ptr<Tower>>& towers)
{
    m_waveManager.update(dt);
    while (m_waveManager.shouldSpawn()) {
        spawnEnemy();
    }

    for (auto& e : m_enemies) {
        if (e->isActive()) e->update(dt);
    }

    for (auto& e : m_enemies) {
        if (e->reachedEnd()) {
            reduceLives(e->damage());
        }
    }

    for (auto& t : towers) {
        t->setPriorityEnemy(m_priorityEnemy);
        t->setPriorityObstacle(m_priorityObstacle);
        t->update(dt, m_enemies);
    }

    for (auto& t : towers) {
        if (MeleeTower* mt = dynamic_cast<MeleeTower*>(t.get())) {
            if (mt->hasPendingEffect()) {
                auto effect = mt->getEffect();
                for (auto& e : m_enemies) {
                    if (!e->isActive()) continue;
                    QPointF ePixel = e->pos(m_spatialGrid->cellSize(), m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
                    QPointF d = ePixel - effect.center;
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
                auto b = createBullet(BulletType::Normal, QPointF(t->gridX(), t->gridY()), attack.targetPos,
                                      attack.damage, attack.splashRadius, attack.slowFactor,
                                      attack.slowDuration, attack.poisonDps, attack.poisonDuration,
                                      attack.chainCount, attack.color);
                b->setMaxDistance(attack.maxDistance);
                b->setCellSize(m_spatialGrid->cellSize());
                b->setOffset(m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
                m_projectiles.push_back(std::move(b));
            }
        }
    }

    if (m_spatialGrid) {
        m_spatialGrid->syncEntityGrid(m_enemies, m_obstacles);
    }

    for (auto& p : m_projectiles) {
        if (p->isActive()) {
            QPointF bp = p->pos();
            QPoint g(static_cast<int>(bp.x()), static_cast<int>(bp.y()));
            CellEntities& cell = m_spatialGrid->getCellAt(g.x(), g.y());
            p->update(dt, m_enemies, cell);
        }
    }

    for (auto& obs : m_obstacles) {
        obs->update(dt);
    }

    for (auto& p : m_projectiles) {
        if (p->hasHit()) {
            handleProjectileHit(*p);
        }
    }

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](auto& e) { return e->isDead() || e->reachedEnd(); }),
        m_enemies.end());
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](auto& p) { return !p->isActive(); }),
        m_projectiles.end());

    if (m_waveManager.waveComplete() && m_enemies.empty()) {
        if (m_waveManager.allWavesDone()) {
            m_victory = true;
            m_gameOver = true;
            emit gameEnded(true, m_levelId);
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

void GameController::spawnEnemy()
{
    EnemyType type = m_waveManager.popSpawnType();
    std::vector<QPointF> waypoints = m_spatialGrid->waypoints();
    m_enemies.push_back(createEnemy(type, waypoints));
}

void GameController::handleProjectileHit(Bullet& proj)
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

void GameController::checkGameEnd()
{
    if (m_lives <= 0) {
        m_lives = 0;
        m_gameOver = true;
        emit gameEnded(false, m_levelId);
    }
}

void GameController::setPriorityEnemy(Enemy* e)
{
    m_priorityEnemy = e;
    m_priorityObstacle = nullptr;
}

void GameController::setPriorityObstacle(Obstacle* obs)
{
    m_priorityObstacle = obs;
    m_priorityEnemy = nullptr;
}

void GameController::clearPriorityTarget()
{
    m_priorityEnemy = nullptr;
    m_priorityObstacle = nullptr;
}

CellEntities GameController::getCellAt(int gx, int gy)
{
    return m_spatialGrid->getCellAt(gx, gy);
}