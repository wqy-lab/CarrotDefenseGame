#include "tower.h"
#include "../enemies/enemy.h"
#include "../obstacles/obstacle.h"
#include "../config/datamanager.h"

Tower::Tower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : m_type(type)
    , m_gridX(gridX), m_gridY(gridY), m_cellSize(cellSize)
{
    TowerStats s1 = DataManager::instance().getTowerStats(type, 1);
    TowerStats s2 = DataManager::instance().getTowerStats(type, 2);
    TowerStats s3 = DataManager::instance().getTowerStats(type, 3);
    m_baseStats[1] = s1;
    m_baseStats[2] = s2;
    m_baseStats[3] = s3;

    for (int lvl = 1; lvl <= 3; ++lvl) {
        auto markerCfgs = DataManager::instance().getTowerMarkers(type, lvl);
        for (const auto& cfg : markerCfgs) {
            if (cfg.type == "slow") {
                m_markerTemplates[lvl].push_back(std::make_unique<SlowMarker>(cfg.factor, cfg.duration));
            } else if (cfg.type == "poison") {
                m_markerTemplates[lvl].push_back(std::make_unique<PoisonMarker>(cfg.factor, cfg.duration));
            }
        }
    }

    m_totalInvested = m_baseStats[1].cost;
}

bool Tower::upgrade() {
    if (m_level >= MAX_LEVEL) return false;
    int cost = upgradeCost();
    m_totalInvested += cost;
    m_level++;
    return true;
}

QString Tower::name() const {
    switch (m_type) {
        case TowerType::Arrow: return "Arrow Tower";
        case TowerType::Cannon: return "Cannon Tower";
        case TowerType::Ice: return "Ice Tower";
        case TowerType::Poison: return "Poison Tower";
        case TowerType::Lightning: return "Lightning Tower";
        case TowerType::Sun: return "Sun Tower";
    }
    return "Unknown";
}

QPointF Tower::centerPos(double cellSize, double offsetX, double offsetY) const
{
    return QPointF(offsetX + (m_gridX + 0.5) * cellSize,
                   offsetY + (m_gridY + 0.5) * cellSize);
}

void Tower::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    double r = cellSize * 0.42;
    QPointF center = centerPos(cellSize, offsetX, offsetY);
    double cx = center.x(), cy = center.y();
    p.setRenderHint(QPainter::Antialiasing, true);

    // Base shadow
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,0,0,60));
    p.drawEllipse(QPointF(cx, cy + r*0.2), r*0.8, r*0.3);

    drawBody(p, center, r);
}

Obstacle* Tower::findObstacleTarget() const
{
    if (m_priorityObstacle && m_priorityObstacle->isActive()) {
        double r2 = range() * range();
        double d2 = distTo(*m_priorityObstacle);
        if (d2 <= r2) {
            return m_priorityObstacle;
        }
    }
    return nullptr;
}

double Tower::distTo(const Obstacle& o) const {
    double dx = (o.gridX() + o.gridWidth() / 2.0) - (m_gridX + 0.5);
    double dy = (o.gridY() + o.gridHeight() / 2.0) - (m_gridY + 0.5);
    return dx*dx + dy*dy;
}

std::vector<std::unique_ptr<Marker>> Tower::cloneMarkers() const {
    std::vector<std::unique_ptr<Marker>> result;
    for (const auto& m : markers()) {
        result.push_back(m->clone());
    }
    return result;
}