#include "lightningbullet.h"
#include "../enemies/enemy.h"
#include <QtMath>
#include <QPainterPath>
#include <cstdlib>

LightningBullet::LightningBullet(const QPointF& start, const QPointF& direction, double damage,
                                  int chainCount, const QColor& color)
    : Bullet(start, direction, damage, color)
    , m_chainCount(chainCount)
{
}

void LightningBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);

    m_chainedEnemies.insert(enemy);
    QPointF origin = enemy->gridPos();

    while (m_chainCount > 0) {
        Enemy* nextTarget = nullptr;
        double minDist = 3.0;
        minDist = minDist * minDist;

        for (auto& e : enemies) {
            if (!e->isActive()) continue;
            if (m_chainedEnemies.count(e.get()) > 0) continue;
            QPointF d = e->gridPos() - origin;
            double dist2 = d.x()*d.x() + d.y()*d.y();
            if (dist2 < minDist) {
                minDist = dist2;
                nextTarget = e.get();
            }
        }
        if (!nextTarget) break;
        m_chainedEnemies.insert(nextTarget);
        nextTarget->takeDamage(m_damage);
        origin = nextTarget->gridPos();
        --m_chainCount;
    }
}

void LightningBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;

    QPointF start(offsetX + m_startPos.x() * cellSize,
                  offsetY + m_startPos.y() * cellSize);
    QPointF end(offsetX + m_pos.x() * cellSize,
                offsetY + m_pos.y() * cellSize);

    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(QColor(100, 180, 255), 2.5, Qt::SolidLine, Qt::RoundCap);
    p.setPen(pen);

    QPointF mid((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);

    int j1 = rand() % 16 - 8;
    int j2 = rand() % 16 - 8;

    QPointF p1(start.x() + (mid.x() - start.x()) / 3.0 + j1,
               start.y() + (mid.y() - start.y()) / 3.0 + j2);
    QPointF p2(mid.x() + j2, mid.y() + j1);
    QPointF p3(mid.x() + (end.x() - mid.x()) / 3.0 * 2.0 + j1,
               mid.y() + (end.y() - mid.y()) / 3.0 * 2.0 - j2);

    QPainterPath path;
    path.moveTo(start);
    path.lineTo(p1);
    path.lineTo(p2);
    path.lineTo(p3);
    path.lineTo(end);
    p.drawPath(path);

    QPen glow(QColor(255, 255, 255, 100), 1.0, Qt::SolidLine, Qt::RoundCap);
    p.setPen(glow);
    p.drawPath(path);
}