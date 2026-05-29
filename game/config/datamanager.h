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

struct MapData
{
    int gridCols = 15;
    int gridRows = 12;
    int startX = 0, startY = 6;
    int endX = 14, endY = 6;
    std::vector<QPoint> pathCells;
};

struct LevelEntry
{
    int id;
    QString name;
    QString file;
};

class DataManager
{
public:
    static DataManager& instance();

    bool loadShared(const QString& path);
    bool loadLevel(const QString& path);
    bool loadLevelsIndex(const QString& path);

    TowerStats getTowerStats(TowerType type) const;
    EnemyStats getEnemyStats(EnemyType type) const;

    const MapData& mapData() const { return m_mapData; }

    int initialGold() const { return m_initialGold; }
    int initialLives() const { return m_initialLives; }
    int waveBonusBase() const { return m_waveBonusBase; }
    int waveBonusPerWave() const { return m_waveBonusPerWave; }

    const std::vector<std::vector<WaveEntry>>& waves() const
    {
        return m_waves;
    }

    const std::vector<LevelEntry>& levels() const
    {
        return m_levels;
    }

private:
    DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    QHash<TowerType, TowerStats> m_towerStats;
    QHash<EnemyType, EnemyStats> m_enemyStats;
    std::vector<std::vector<WaveEntry>> m_waves;
    std::vector<LevelEntry> m_levels;
    MapData m_mapData;

    int m_initialGold = 200;
    int m_initialLives = 10;
    int m_waveBonusBase = 30;
    int m_waveBonusPerWave = 5;
};

#endif // DATAMANAGER_H
