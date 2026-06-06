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
#include "towers/arrowtower.h"
#include "towers/canntower.h"
#include "towers/icetower.h"
#include "towers/poisontower.h"
#include "towers/lighttower.h"
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
    clearPriorityTarget();
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
    dt *= m_timeScale;
    if (dt <= 0) return;
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
        if (m_priorityObstacle) {
            t->setPriorityObstacle(m_priorityObstacle);
        } else if (m_priorityEnemy) {
            t->setPriorityEnemy(m_priorityEnemy);
        } else {
            t->setPriorityEnemy(nullptr);
            t->setPriorityObstacle(nullptr);
        }
        t->update(dt, m_enemies);
    }

    for (auto& t : towers) {
        if (MeleeTower* mt = dynamic_cast<MeleeTower*>(t.get())) {
            if (mt->hasPendingEffect()) {
                auto effect = mt->getEffect();
                // effect.center is in grid units, convert to pixels
                double centerX = m_spatialGrid->offsetX() + effect.center.x() * m_spatialGrid->cellSize() + m_spatialGrid->cellSize() / 2.0;
                double centerY = m_spatialGrid->offsetY() + effect.center.y() * m_spatialGrid->cellSize() + m_spatialGrid->cellSize() / 2.0;
                QPointF effectCenterPixel(centerX, centerY);
                double effectRadiusPx = effect.radius * m_spatialGrid->cellSize();
                for (auto& e : m_enemies) {
                    if (!e->isActive()) continue;
                    QPointF ePixel = e->pos(m_spatialGrid->cellSize(), m_spatialGrid->offsetX(), m_spatialGrid->offsetY());
                    QPointF d = ePixel - effectCenterPixel;
                    double dist = std::sqrt(d.x()*d.x() + d.y()*d.y());
                    if (dist <= effectRadiusPx) {
                        double falloff = 1.0 - (dist / effectRadiusPx) * 0.5;
                        e->takeDamage(effect.damage * falloff);
                        for (auto& m : effect.markers) {
                            e->addMarker(m->clone());
                        }
                    }
                }
            }
        } else if (RemoteTower* rt = dynamic_cast<RemoteTower*>(t.get())) {
            if (rt->hasPendingAttack()) {
                auto attack = rt->getAttack();
                BulletType btype = BulletType::Arrow;
                if (ArrowTower* at = dynamic_cast<ArrowTower*>(t.get())) {
                    btype = BulletType::Arrow;
                } else if (CannonTower* ct = dynamic_cast<CannonTower*>(t.get())) {
                    btype = BulletType::Cannon;
                } else if (IceTower* it = dynamic_cast<IceTower*>(t.get())) {
                    btype = BulletType::Ice;
                } else if (PoisonTower* pt = dynamic_cast<PoisonTower*>(t.get())) {
                    btype = BulletType::Poison;
                } else if (LightningTower* lt = dynamic_cast<LightningTower*>(t.get())) {
                    btype = BulletType::Lightning;
                }
                auto b = createBullet(btype, QPointF(t->gridX() + 0.5, t->gridY() + 0.5), attack.targetPos,
                                      attack.damage, attack.splashRadius, attack.color,
                                      std::move(attack.markers));
                b->setMaxDistance(attack.maxDistance);
                b->setGridBounds(m_spatialGrid->gridCols(), m_spatialGrid->gridRows());
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
            int gx = static_cast<int>(std::floor(bp.x()));
            int gy = static_cast<int>(std::floor(bp.y()));
            CellEntities& cell = m_spatialGrid->getCellAt(gx, gy);
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

    // Notify tutorial if any enemy was killed this frame
    if (m_onEnemyKilled) {
        for (auto& e : m_enemies) {
            if (e->isDead()) {
                m_onEnemyKilled();
                break;
            }
        }
    }

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](auto& e) { return e->isDead() || e->reachedEnd(); }),
        m_enemies.end());

    // Clear priority if target was removed
    if (m_priorityEnemy) {
        bool found = false;
        for (auto& e : m_enemies) {
            if (e.get() == m_priorityEnemy) { found = true; break; }
        }
        if (!found) m_priorityEnemy = nullptr;
    }
    if (m_priorityObstacle) {
        bool found = false;
        for (auto& o : m_obstacles) {
            if (o.get() == m_priorityObstacle) { found = true; break; }
        }
        if (!found) m_priorityObstacle = nullptr;
    }

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
