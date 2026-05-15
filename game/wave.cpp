#include "wave.h"
#include "enemies/enemyfactory.h"
#include "config/datamanager.h"

WaveManager::WaveManager()
    : m_currentWave(0), m_spawnTimer(0), m_spawnedAll(false)
{
    buildWaves();
}

void WaveManager::buildWaves()
{
    m_waves = DataManager::instance().waves();
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
