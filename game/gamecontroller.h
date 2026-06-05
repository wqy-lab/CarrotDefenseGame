#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <vector>
#include <memory>
#include <functional>
#include "wave.h"
#include "cellentities.h"
#include "enemies/enemy.h"
#include "bullets/bullet.h"
#include "obstacles/obstacle.h"
#include "towers/tower.h"
#include "spatialgrid.h"

class GameController : public QObject {
    Q_OBJECT

public:
    explicit GameController(SpatialGrid* spatialGrid, QObject* parent = nullptr);
    ~GameController() = default;

    void setOnEnemyKilled(std::function<void()> callback) { m_onEnemyKilled = std::move(callback); }

    void startGame();
    void pauseGame();
    void resumeGame();
    void resetGame();

    bool isRunning() const { return m_gameRunning; }
    bool isPaused() const { return m_paused; }
    bool isGameOver() const { return m_gameOver; }
    bool isVictory() const { return m_victory; }
    int gold() const { return m_gold; }
    int lives() const { return m_lives; }
    int levelId() const { return m_levelId; }
    int currentWave() const { return m_waveManager.currentWave(); }
    int totalWaves() const { return m_waveManager.totalWaves(); }
    int enemiesInWave() const { return m_waveManager.enemiesRemainingInWave(); }

    void setLevelId(int id) { m_levelId = id; }
    void setPriorityEnemy(Enemy* e);
    void setPriorityObstacle(Obstacle* obs);
    void clearPriorityTarget();
    Enemy* priorityEnemy() const { return m_priorityEnemy; }
    Obstacle* priorityObstacle() const { return m_priorityObstacle; }

    void addGold(int amount) { m_gold += amount; }
    void spendGold(int amount) { m_gold -= amount; }
    void reduceLives(int amount);
    void update(double dt, const std::vector<std::unique_ptr<Tower>>& towers);

    std::vector<std::unique_ptr<Enemy>>& enemies() { return m_enemies; }
    std::vector<std::unique_ptr<Bullet>>& projectiles() { return m_projectiles; }
    std::vector<std::unique_ptr<Obstacle>>& obstacles() { return m_obstacles; }

signals:
    void statsChanged();
    void gameEnded(bool won, int levelId);

private:
    void updateGame(double dt, const std::vector<std::unique_ptr<Tower>>& towers);
    void spawnEnemy();
    void handleProjectileHit(Bullet& proj);
    void checkGameEnd();
    CellEntities getCellAt(int gx, int gy);

    bool m_gameRunning = false;
    bool m_paused = false;
    bool m_gameOver = false;
    bool m_victory = false;
    int m_gold = 200;
    int m_lives = 10;
    int m_levelId = 0;

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Bullet>> m_projectiles;
    std::vector<std::unique_ptr<Obstacle>> m_obstacles;
    WaveManager m_waveManager;
    Enemy* m_priorityEnemy = nullptr;
    Obstacle* m_priorityObstacle = nullptr;

    SpatialGrid* m_spatialGrid = nullptr;
    std::function<void()> m_onEnemyKilled;
};

#endif