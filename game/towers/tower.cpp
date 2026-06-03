#include "tower.h"
#include "../enemies/enemy.h"
#include "../config/datamanager.h"
#include <cmath>

Tower::Tower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : m_type(type), m_stats()
    , m_gridX(gridX), m_gridY(gridY), m_cellSize(cellSize)
{
    m_center = QPointF(offsetX + gridX*cellSize + cellSize/2.0,
                       offsetY + gridY*cellSize + cellSize/2.0);
    m_baseStats = DataManager::instance().getTowerStats(type);
    m_stats = m_baseStats;
    m_baseCost = m_baseStats.cost;
    m_level = 1;
    m_totalInvested = m_baseCost;
}

bool Tower::upgrade() {
    if (m_level >= 10) return false;
    int cost = upgradeCost();
    m_totalInvested += cost;
    m_level++;
    double lvl = m_level - 1;
    m_stats.damage = m_baseStats.damage * std::pow(1.15, lvl);
    m_stats.range = m_baseStats.range * std::pow(1.05, lvl);
    m_stats.attackSpeed = m_baseStats.attackSpeed * std::pow(1.10, lvl);
    m_stats.splashRadius = m_baseStats.splashRadius * std::pow(1.10, lvl);
    m_stats.slowFactor = m_baseStats.slowFactor * std::pow(1.10, lvl);
    m_stats.slowDuration = m_baseStats.slowDuration * std::pow(1.10, lvl);
    m_stats.poisonDps = m_baseStats.poisonDps * std::pow(1.10, lvl);
    m_stats.poisonDuration = m_baseStats.poisonDuration * std::pow(1.10, lvl);
    m_stats.chainCount = m_baseStats.chainCount + (m_level > 1 ? 1 : 0);
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

void Tower::updateCenter(double cellSize, double offsetX, double offsetY)
{
    m_cellSize = cellSize;
    m_center = QPointF(offsetX + m_gridX * cellSize + cellSize / 2.0,
                       offsetY + m_gridY * cellSize + cellSize / 2.0);
}

void Tower::draw(QPainter& p) const
{
    double r = m_cellSize * 0.42;
    double cx = m_center.x(), cy = m_center.y();
    p.setRenderHint(QPainter::Antialiasing, true);

    if (!m_stats.texture.isNull()) {
        QRectF target(cx - r, cy - r, r * 2, r * 2);
        p.drawPixmap(target.toRect(), m_stats.texture);
        return;
    }

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,0,0,60));
    p.drawEllipse(QPointF(cx, cy + r*0.2), r*0.8, r*0.3);

    drawBody(p, m_center, r);
}