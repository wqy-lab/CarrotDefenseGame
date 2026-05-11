#include "wave.h"
#include "enemies/enemyfactory.h"

WaveManager::WaveManager()
    : m_currentWave(0), m_spawnTimer(0), m_spawnedAll(false)
{
    buildWaves();
}

void WaveManager::buildWaves()
{
    // Wave 1: easy intro
    m_waves.push_back({{ EnemyType::Normal, 6, 1.0 }});
    // Wave 2
    m_waves.push_back({{ EnemyType::Normal, 10, 0.9 }});
    // Wave 3: Fast + Swarm
    m_waves.push_back({{ EnemyType::Normal, 6, 0.8 }, { EnemyType::Fast, 3, 1.5 }, { EnemyType::Swarm, 5, 0.4 }});
    // Wave 4
    m_waves.push_back({{ EnemyType::Normal, 8, 0.7 }, { EnemyType::Fast, 6, 1.2 }, { EnemyType::Swarm, 8, 0.3 }});
    // Wave 5: Tank
    m_waves.push_back({{ EnemyType::Normal, 8, 0.6 }, { EnemyType::Fast, 5, 1.0 }, { EnemyType::Tank, 2, 2.5 }});
    // Wave 6: hard mix
    m_waves.push_back({{ EnemyType::Normal, 10, 0.5 }, { EnemyType::Fast, 8, 0.9 }, { EnemyType::Tank, 3, 2.0 }, { EnemyType::Swarm, 10, 0.3 }});
    // Wave 7: Boss!
    m_waves.push_back({{ EnemyType::Tank, 4, 1.5 }, { EnemyType::Fast, 8, 0.7 }, { EnemyType::Boss, 1, 5.0 }});
    // Wave 8: final
    m_waves.push_back({{ EnemyType::Normal, 15, 0.4 }, { EnemyType::Fast, 10, 0.7 }, { EnemyType::Tank, 5, 1.5 }, { EnemyType::Swarm, 15, 0.25 }, { EnemyType::Boss, 2, 4.0 }});
}

void WaveManager::reset()
{
    m_currentWave = 0;
    m_spawnQueue.clear();
    m_spawnTimer = 0;
    m_spawnedAll = true;
}

void WaveManager::nextWave()
{
    if (m_currentWave >= static_cast<int>(m_waves.size())) return;
    auto& entries = m_waves[m_currentWave];
    m_spawnQueue.clear();
    for (auto& entry : entries)
        for (int i = 0; i < entry.count; ++i)
            m_spawnQueue.push_back(entry.type);
    m_spawnTimer = 1.5;
    m_spawnedAll = false;
    ++m_currentWave;
}

void WaveManager::update(double dt)
{
    if (m_spawnedAll || m_spawnQueue.empty()) return;
    m_spawnTimer -= dt;
    if (m_spawnQueue.empty()) m_spawnedAll = true;
}

EnemyType WaveManager::popSpawnType()
{
    EnemyType t = m_spawnQueue.front();
    m_spawnQueue.erase(m_spawnQueue.begin());
    m_spawnTimer = 0.8;
    if (m_spawnQueue.empty()) m_spawnedAll = true;
    return t;
}
