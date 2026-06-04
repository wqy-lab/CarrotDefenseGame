#ifndef WAVE_H
#define WAVE_H

#include "enemies/enemyfactory.h"
#include <vector>
#include <utility>

struct WaveEntry {
    EnemyType type;
    int count;
    double interval; // seconds between spawns
};

class WaveManager {
public:
    WaveManager();

    void reset();
    void nextWave();

    int currentWave() const { return m_currentWave; }
    int totalWaves() const { return static_cast<int>(m_waves.size()); }
    bool allWavesDone() const { return m_currentWave >= totalWaves() && m_spawnQueue.empty(); }
    bool waveComplete() const { return m_spawnedAll && m_spawnQueue.empty(); }

    void update(double dt);
    bool shouldSpawn() const { return m_spawnTimer <= 0 && !m_spawnQueue.empty(); }
    EnemyType popSpawnType();

    // Get the wave data for status display
    int enemiesRemainingInWave() const { return static_cast<int>(m_spawnQueue.size()); }

private:
    void buildWaves();

    int m_currentWave;
    std::vector<std::vector<WaveEntry>> m_waves;

    // Spawn state — pair stores (enemyType, interval in seconds)
    std::vector<std::pair<EnemyType, double>> m_spawnQueue;
    double m_spawnTimer;
    bool m_spawnedAll;
};

#endif // WAVE_H
