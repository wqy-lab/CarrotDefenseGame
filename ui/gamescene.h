#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QMouseEvent>
#include <vector>
#include <memory>
#include <set>
#include "../game/enemies/enemy.h"
#include "../game/towers/tower.h"
#include "../game/towers/meleetower.h"
#include "../game/towers/remotetower.h"
#include "../game/bullets/bullet.h"
#include "../game/wave.h"
#include "../game/config/datamanager.h"
#include "../game/obstacles/obstacle.h"
#include "../game/cellentities.h"

class GameScene : public QWidget {
    Q_OBJECT

public:
    explicit GameScene(QWidget* parent = nullptr);

    void startGame();
    void pauseGame();
    void resumeGame();
    void resetGame();
    bool isPaused() const { return m_paused; }
    bool isRunning() const { return m_gameRunning; }

    void selectTowerType(TowerType type);
    TowerType selectedTower() const { return m_selectedTower; }

    void setLevelId(int id) { m_levelId = id; }
    int levelId() const { return m_levelId; }

    int gold() const { return m_gold; }
    int lives() const { return m_lives; }
    int currentWave() const { return m_waveManager.currentWave(); }
    int totalWaves() const { return m_waveManager.totalWaves(); }
    int enemiesInWave() const { return m_waveManager.enemiesRemainingInWave(); }
    bool gameOver() const { return m_gameOver; }
    bool victory() const { return m_victory; }

signals:
    void statsChanged();
    void gameEnded(bool won, int levelId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void gameLoop();

private:
    int m_gridCols;
    int m_gridRows;
    int m_startX, m_startY;
    int m_endX, m_endY;

    double m_cellSize;
    double m_offsetX, m_offsetY;

    bool m_gameRunning;
    bool m_paused;
    bool m_gameOver;
    bool m_victory;
    int m_gold;
    int m_lives;

    std::vector<std::unique_ptr<Tower>> m_towers;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Bullet>> m_projectiles;
    std::vector<std::unique_ptr<Obstacle>> m_obstacles;
    WaveManager m_waveManager;
    Enemy* m_priorityEnemy = nullptr;
    Obstacle* m_priorityObstacle = nullptr;

    // Fixed path: cells that are the road (enemies walk here, NO towers)
    std::vector<std::vector<bool>> m_isPath;
    // Grid cells occupied by active obstacles (NO towers)
    std::vector<std::vector<bool>> m_obstacleCell;
    std::vector<std::vector<CellEntities>> m_entityGrid;
    // Pre-computed pixel waypoints for the winding path
    std::vector<QPointF> m_waypoints;
    // Grid coords of path (for fast lookup)
    std::set<std::pair<int,int>> m_pathSet;

    TowerType m_selectedTower;
    bool m_placingTower;
    int m_hoverGridX, m_hoverGridY;
    bool m_showRange;
    int m_levelId = 0;

    QTimer* m_gameTimer;
    QElapsedTimer m_clock;

    void drawGrid(QPainter& p);
    void drawPath(QPainter& p);
    void drawTowers(QPainter& p);
    void drawEnemies(QPainter& p);
    void drawProjectiles(QPainter& p);
    void drawObstacles(QPainter& p);
    void updateObstacles(double dt);
    void syncEntityGrid();
    CellEntities& getCellAt(int gx, int gy);

    void setPriorityTarget(Enemy* e);
    void setPriorityTarget(Obstacle* obs);
    void clearPriorityTarget();
    Enemy* priorityEnemy() const { return m_priorityEnemy; }
    Obstacle* priorityObstacle() const { return m_priorityObstacle; }

    void updateGame(double dt);
    void initMap(const MapData& map);
    void placeTower(int gx, int gy);
    void spawnEnemy();
    void handleProjectileHit(Bullet& proj);
    void checkGameEnd();

    QPointF gridToPixel(int gx, int gy) const;
    QPoint pixelToGrid(const QPointF& pos) const;
    bool isValidGridPos(int gx, int gy) const;
    bool isPathCell(int gx, int gy) const;
    bool isObstacleCell(int gx, int gy) const;
};

#endif // GAMESCENE_H
