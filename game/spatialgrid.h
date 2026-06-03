#ifndef SPATIALGRID_H
#define SPATIALGRID_H

#include <QPointF>
#include <vector>
#include <memory>
#include <set>

class CellEntities;
class MapData;
class Obstacle;

class SpatialGrid {
public:
    SpatialGrid();

    void initMap(const MapData& map);

    int gridCols() const { return m_gridCols; }
    int gridRows() const { return m_gridRows; }
    int startX() const { return m_startX; }
    int startY() const { return m_startY; }
    int endX() const { return m_endX; }
    int endY() const { return m_endY; }
    double cellSize() const { return m_cellSize; }
    double offsetX() const { return m_offsetX; }
    double offsetY() const { return m_offsetY; }
    const std::vector<QPointF>& waypoints() const { return m_waypoints; }

    QPointF gridToPixel(int gx, int gy) const;
    QPoint pixelToGrid(const QPointF& pos) const;
    bool isValidGridPos(int gx, int gy) const;
    bool isPathCell(int gx, int gy) const;
    bool isObstacleCell(int gx, int gy) const;

    CellEntities& getCellAt(int gx, int gy);
    void syncEntityGrid(const std::vector<std::unique_ptr<class Enemy>>& enemies,
                       const std::vector<std::unique_ptr<Obstacle>>& obstacles);
    void repositionObstacles(std::vector<std::unique_ptr<Obstacle>>& obstacles);

    void setCellSize(double size) { m_cellSize = size; }
    void setOffset(double ox, double oy) { m_offsetX = ox; m_offsetY = oy; }
    void rebuildWaypoints(const std::vector<QPointF>& waypoints);
    void clearObstacles();

private:
    int m_gridCols = 0;
    int m_gridRows = 0;
    int m_startX = 0, m_startY = 0;
    int m_endX = 0, m_endY = 0;
    double m_cellSize = 48;
    double m_offsetX = 0, m_offsetY = 0;

    std::vector<std::vector<bool>> m_isPath;
    std::vector<std::vector<bool>> m_obstacleCell;
    std::vector<std::vector<CellEntities>> m_entityGrid;
    std::vector<QPointF> m_waypoints;
    std::set<std::pair<int,int>> m_pathSet;
};

#endif