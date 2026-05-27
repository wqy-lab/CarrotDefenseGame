#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QHash>
#include <QString>
#include <QPoint>
#include <vector>
#include "../towers/tower.h"
#include "../enemies/enemy.h"
#include "../enemies/enemyfactory.h"
#include "../wave.h"
#include "../obstacles/obstaclefactory.h"

struct ObstacleEntry {
    ObstacleType type;
    int gridX;
    int gridY;
    int gridW;
    int gridH;
};

struct ObstacleStats
{
    double maxHp;
    int reward;
    QColor color;
    int radius;
};

struct MapData
{
    int gridCols = 15;
    int gridRows = 12;
    int startX = 0, startY = 6;
    int endX = 14, endY = 6;
    std::vector<QPoint> pathCells;
};

class DataManager
{
public:
    static DataManager& instance();

    bool loadShared(const QString& path);
    bool loadLevel(const QString& path);

    TowerStats getTowerStats(TowerType type) const;
    EnemyStats getEnemyStats(EnemyType type) const;

    const MapData& mapData() const { return m_mapData; }

    int initialGold() const { return m_initialGold; }
    int initialLives() const { return m_initialLives; }
    int waveBonusBase() const { return m_waveBonusBase; }
    int waveBonusPerWave() const { return m_waveBonusPerWave; }

    ObstacleStats getObstacleStats(ObstacleType type) const;
    const std::vector<ObstacleEntry>& obstacles() const { return m_obstacles; }

    const std::vector<std::vector<WaveEntry>>& waves() const
    {
        return m_waves;
    }

private:
    DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    QHash<TowerType, TowerStats> m_towerStats;
    QHash<EnemyType, EnemyStats> m_enemyStats;
    QHash<ObstacleType, ObstacleStats> m_obstacleStats;
    std::vector<ObstacleEntry> m_obstacles;
    std::vector<std::vector<WaveEntry>> m_waves;
    MapData m_mapData;

    int m_initialGold = 200;
    int m_initialLives = 10;
    int m_waveBonusBase = 30;
    int m_waveBonusPerWave = 5;
};

#endif // DATAMANAGER_H
