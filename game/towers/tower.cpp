#include "tower.h"
#include "../enemies/enemy.h"

Tower::Tower(TowerType type, int gridX, int gridY, double cellSize, double offsetX, double offsetY)
    : m_type(type), m_stats()
    , m_gridX(gridX), m_gridY(gridY), m_cellSize(cellSize)
{
    m_center = QPointF(offsetX + gridX*cellSize + cellSize/2.0,
                       offsetY + gridY*cellSize + cellSize/2.0);
}

void Tower::draw(QPainter& p) const
{
    double r = m_cellSize * 0.42;
    double cx = m_center.x(), cy = m_center.y();
    p.setRenderHint(QPainter::Antialiasing, true);

    // Base shadow
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,0,0,60));
    p.drawEllipse(QPointF(cx, cy + r*0.2), r*0.8, r*0.3);

    drawBody(p, m_center, r);
}