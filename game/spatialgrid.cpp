#include "spatialgrid.h"
#include "cellentities.h"
#include "config/datamanager.h"
#include "enemies/enemy.h"
#include "obstacles/obstacle.h"
#include <algorithm>
#include <cmath>

SpatialGrid::SpatialGrid()
{
}

void SpatialGrid::initMap(const MapData& map)
{
    m_gridCols = map.gridCols;
    m_gridRows = map.gridRows;
    m_startX = map.startX;
    m_startY = map.startY;
    m_endX = map.endX;
    m_endY = map.endY;

    m_isPath.assign(m_gridRows, std::vector<bool>(m_gridCols, false));
    m_obstacleCell.assign(m_gridRows, std::vector<bool>(m_gridCols, false));
    m_entityGrid.assign(m_gridRows, std::vector<CellEntities>(m_gridCols));
    m_pathSet.clear();
    m_pathCells.clear();

    for (const QPoint& p : map.pathCells) {
        m_pathSet.insert({p.x(), p.y()});
        m_isPath[p.y()][p.x()] = true;
        m_pathCells.push_back(p);
    }
}

std::vector<QPointF> SpatialGrid::waypoints() const
{
    std::vector<QPointF> result;
    result.reserve(m_pathCells.size());
    for (const QPoint& p : m_pathCells) {
        result.push_back(QPointF(p.x(), p.y()));
    }
    return result;
}

QPointF SpatialGrid::gridToPixel(int gx, int gy) const
{
    return QPointF(
        m_offsetX + gx * m_cellSize + m_cellSize / 2.0,
        m_offsetY + gy * m_cellSize + m_cellSize / 2.0);
}

QPointF SpatialGrid::pixelToGrid(const QPointF& pos) const
{
    return QPointF(
        (pos.x() - m_offsetX) / m_cellSize,
        (pos.y() - m_offsetY) / m_cellSize);
}

bool SpatialGrid::isValidGridPos(int gx, int gy) const
{
    return gx >= 0 && gx < m_gridCols && gy >= 0 && gy < m_gridRows;
}

bool SpatialGrid::isPathCell(int gx, int gy) const
{
    if (!isValidGridPos(gx, gy)) return true;
    return m_isPath[gy][gx];
}

bool SpatialGrid::isObstacleCell(int gx, int gy) const
{
    if (!isValidGridPos(gx, gy)) return false;
    return m_obstacleCell[gy][gx];
}

CellEntities& SpatialGrid::getCellAt(int gx, int gy)
{
    static CellEntities dummy;
    if (!isValidGridPos(gx, gy)) return dummy;
    return m_entityGrid[gy][gx];
}

void SpatialGrid::syncEntityGrid(const std::vector<std::unique_ptr<Enemy>>& enemies,
                                 const std::vector<std::unique_ptr<Obstacle>>& obstacles)
{
    for (auto& row : m_entityGrid) {
        for (auto& cell : row) {
            cell.enemies.clear();
            cell.obstacles.clear();
        }
    }

    for (auto& e : enemies) {
        if (!e->isActive()) continue;
        QPointF gp = e->gridPos();
        int gx = static_cast<int>(std::floor(gp.x()));
        int gy = static_cast<int>(std::floor(gp.y()));
        if (isValidGridPos(gx, gy)) {
            m_entityGrid[gy][gx].enemies.push_back(e.get());
        }
    }

    for (auto& obs : obstacles) {
        if (!obs->isActive()) continue;
        for (int dy = 0; dy < obs->gridHeight(); ++dy) {
            for (int dx = 0; dx < obs->gridWidth(); ++dx) {
                int cx = obs->gridX() + dx;
                int cy = obs->gridY() + dy;
                if (isValidGridPos(cx, cy)) {
                    m_entityGrid[cy][cx].obstacles.push_back(obs.get());
                }
            }
        }
    }
}

void SpatialGrid::clearObstacles()
{
    for (auto& row : m_obstacleCell) {
        std::fill(row.begin(), row.end(), false);
    }
}