#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// --- string → enum helpers (file-local) ---

static TowerType stringToTowerType(const QString& s)
{
    if (s == "Arrow") return TowerType::Arrow;
    if (s == "Cannon") return TowerType::Cannon;
    if (s == "Ice") return TowerType::Ice;
    if (s == "Poison") return TowerType::Poison;
    if (s == "Lightning") return TowerType::Lightning;
    if (s == "Sun") return TowerType::Sun;
    return TowerType::Arrow;
}

static EnemyType stringToEnemyType(const QString& s)
{
    if (s == "Normal") return EnemyType::Normal;
    if (s == "Fast") return EnemyType::Fast;
    if (s == "Tank") return EnemyType::Tank;
    if (s == "Boss") return EnemyType::Boss;
    if (s == "Swarm") return EnemyType::Swarm;
    return EnemyType::Normal;
}

static ObstacleType stringToObstacleType(const QString& s)
{
    if (s == "Tree") return ObstacleType::Tree;
    if (s == "Statue") return ObstacleType::Statue;
    return ObstacleType::Tree;
}

// --- singleton ---

DataManager& DataManager::instance()
{
    static DataManager dm;
    return dm;
}

// --- loadShared (towers + enemies) ---

bool DataManager::loadShared(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull())
        return false;

    QJsonObject root = doc.object();

    m_towerStats.clear();
    m_enemyStats.clear();
    m_obstacleStats.clear();

    // --- towers ---
    QJsonArray towers = root["towers"].toArray();
    for (const QJsonValue& val : towers)
    {
        QJsonObject obj = val.toObject();
        TowerType type = stringToTowerType(obj["type"].toString());

        TowerStats s;
        s.cost           = obj["cost"].toInt();
        s.damage         = obj["damage"].toDouble();
        s.range          = obj["range"].toDouble();  // Already in grid units
        s.attackSpeed    = obj["attackSpeed"].toDouble();
        s.splashRadius   = obj["splashRadius"].toDouble(0);  // Already in grid units
        s.slowFactor     = obj["slowFactor"].toDouble(1.0);
        s.slowDuration   = obj["slowDuration"].toDouble(0);
        s.poisonDps      = obj["poisonDps"].toDouble(0);
        s.poisonDuration = obj["poisonDuration"].toDouble(0);
        s.chainCount     = obj["chainCount"].toInt(0);
        s.color          = QColor(obj["color"].toString());

        m_towerStats[type] = s;
    }

    // --- enemies ---
    QJsonArray enemies = root["enemies"].toArray();
    for (const QJsonValue& val : enemies)
    {
        QJsonObject obj = val.toObject();
        EnemyType type = stringToEnemyType(obj["type"].toString());

        EnemyStats s;
        s.maxHp  = obj["maxHp"].toDouble();
        s.speed  = obj["speed"].toDouble() / 48.0;  // Convert to grids/sec
        s.reward = obj["reward"].toInt();
        s.damage = obj["damage"].toInt();
        s.radius = obj["radius"].toInt(10);
        s.color  = QColor(obj["color"].toString());

        m_enemyStats[type] = s;
    }

    // --- obstacles ---
    QJsonArray obstacles = root["obstacles"].toArray();
    for (const QJsonValue& val : obstacles) {
        QJsonObject obj = val.toObject();
        ObstacleType type = stringToObstacleType(obj["type"].toString());

        ObstacleStats s;
        s.maxHp  = obj["maxHp"].toDouble();
        s.reward = obj["reward"].toInt();
        s.radius = obj["radius"].toInt(16);
        s.color  = QColor(obj["color"].toString());

        m_obstacleStats[type] = s;
    }

    return true;
}

// --- loadLevel (map + waves + settings) ---

bool DataManager::loadLevel(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull())
        return false;

    QJsonObject root = doc.object();

    // --- map ---
    QJsonObject mapObj = root["map"].toObject();
    m_mapData.gridCols = mapObj["gridCols"].toInt(15);
    m_mapData.gridRows = mapObj["gridRows"].toInt(12);
    m_mapData.startX = mapObj["startX"].toInt(0);
    m_mapData.startY = mapObj["startY"].toInt(6);
    m_mapData.endX = mapObj["endX"].toInt(14);
    m_mapData.endY = mapObj["endY"].toInt(6);

    m_mapData.pathCells.clear();
    QJsonArray pathArr = mapObj["path"].toArray();
    for (const QJsonValue& pv : pathArr)
    {
        QJsonArray xy = pv.toArray();
        if (xy.size() >= 2)
            m_mapData.pathCells.push_back(
                QPoint(xy[0].toInt(), xy[1].toInt()));
    }

    // --- settings ---
    QJsonObject settings = root["settings"].toObject();
    m_initialGold = settings["initialGold"].toInt(200);
    m_initialLives = settings["initialLives"].toInt(10);
    m_waveBonusBase = settings["waveBonusBase"].toInt(30);
    m_waveBonusPerWave = settings["waveBonusPerWave"].toInt(5);

    // --- waves ---
    m_waves.clear();
    QJsonArray waves = root["waves"].toArray();
    for (const QJsonValue& wv : waves)
    {
        QJsonArray entries = wv.toArray();
        std::vector<WaveEntry> wave;
        for (const QJsonValue& ev : entries)
        {
            QJsonObject obj = ev.toObject();
            WaveEntry entry;
            entry.type     = stringToEnemyType(obj["type"].toString());
            entry.count    = obj["count"].toInt();
            entry.interval = obj["interval"].toDouble();
            wave.push_back(entry);
        }
        m_waves.push_back(std::move(wave));
    }

    // --- obstacles ---
    m_obstacles.clear();
    QJsonArray obstacles = root["obstacles"].toArray();
    for (const QJsonValue& val : obstacles) {
        QJsonObject obj = val.toObject();
        ObstacleEntry entry;
        entry.type = stringToObstacleType(obj["type"].toString());
        entry.gridX = obj["x"].toInt();
        entry.gridY = obj["y"].toInt();
        entry.gridW = obj["w"].toInt(1);
        entry.gridH = obj["h"].toInt(1);
        m_obstacles.push_back(entry);
    }

    return true;
}

// --- loadLevelsIndex (level catalog) ---

bool DataManager::loadLevelsIndex(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull())
        return false;

    m_levels.clear();
    QJsonArray list = doc.object()["levels"].toArray();
    for (const QJsonValue& val : list)
    {
        QJsonObject obj = val.toObject();
        LevelEntry entry;
        entry.id   = obj["id"].toInt();
        entry.name = obj["name"].toString();
        entry.file = obj["file"].toString();
        m_levels.push_back(entry);
    }

    return true;
}

// --- lookup ---

TowerStats DataManager::getTowerStats(TowerType type) const
{
    return m_towerStats.value(type);
}

EnemyStats DataManager::getEnemyStats(EnemyType type) const
{
    return m_enemyStats.value(type);
}

ObstacleStats DataManager::getObstacleStats(ObstacleType type) const
{
    return m_obstacleStats.value(type);
}

const QPixmap& DataManager::getTexture(const QString& path) const
{
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end())
        return it.value();

    QString absPath = QFileInfo(path).absoluteFilePath();
    bool exists = QFile::exists(path);
    qDebug() << "[DataManager] cwd:" << QDir::currentPath()
             << "| requested:" << path
             << "| absolute:" << absPath
             << "| exists:" << exists;

    QPixmap pm(path);
    if (pm.isNull()) {
        m_textureCache.insert(path, QPixmap());
        static QPixmap empty;
        return empty;
    }
    auto result = m_textureCache.insert(path, pm);
    return result.value();
}
