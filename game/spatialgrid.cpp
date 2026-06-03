#include "spatialgrid.h"
#include "cellentities.h"
#include "config/datamanager.h"
#include "enemies/enemy.h"
#include "obstacles/obstacle.h"
#include <algorithm>

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
    m_waypoints.clear();
    m_pathGridCells.clear();

    for (const QPoint& p : map.pathCells) {
        m_pathSet.insert({p.x(), p.y()});
        m_isPath[p.y()][p.x()] = true;
        m_pathGridCells.push_back(p);
        m_waypoints.push_back(gridToPixel(p.x(), p.y()));
    }
}

QPointF SpatialGrid::gridToPixel(int gx, int gy) const
{
    return QPointF(
        m_offsetX + gx * m_cellSize + m_cellSize / 2.0,
        m_offsetY + gy * m_cellSize + m_cellSize / 2.0);
}

QPoint SpatialGrid::pixelToGrid(const QPointF& pos) const
{
    return QPoint(
        static_cast<int>((pos.x() - m_offsetX) / m_cellSize),
        static_cast<int>((pos.y() - m_offsetY) / m_cellSize));
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
        QPoint g = pixelToGrid(e->pos());
        if (isValidGridPos(g.x(), g.y())) {
            m_entityGrid[g.y()][g.x()].enemies.push_back(e.get());
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

void SpatialGrid::repositionObstacles(std::vector<std::unique_ptr<Obstacle>>& obstacles)
{
    for (auto& obs : obstacles) {
        QPointF newPos = gridToPixel(obs->gridX() + obs->gridWidth() / 2,
                                     obs->gridY() + obs->gridHeight() / 2);
        obs->setPosition(newPos);
    }
}

void SpatialGrid::rebuildWaypoints(const std::vector<QPointF>& waypoints)
{
    m_waypoints = waypoints;
}

void SpatialGrid::clearObstacles()
{
    for (auto& row : m_obstacleCell) {
        std::fill(row.begin(), row.end(), false);
    }
}

void SpatialGrid::recomputeWaypoints()
{
    m_waypoints.clear();
    for (const QPoint& p : m_pathGridCells) {
        m_waypoints.push_back(gridToPixel(p.x(), p.y()));
    }
}