#include "lightningbullet.h"
#include "../enemies/enemy.h"
#include <QtMath>
#include <QPainterPath>
#include <cstdlib>

LightningBullet::LightningBullet(const QPointF& start, const QPointF& direction, double damage,
                                  int chainCount, const QColor& color)
    : Bullet(start, direction, damage, color)
    , m_chainCount(chainCount)
    , m_lifetime(0.3)
    , m_lifetimeLeft(0.3)
{
    m_chainPositions.push_back(start);  // 记录起始位置
}

void LightningBullet::update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Bullet::update(dt, enemies, cell);
    m_lifetimeLeft -= dt;
    if (m_lifetimeLeft <= 0) {
        m_active = false;
    }
}

void LightningBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);

    m_chainedEnemies.insert(enemy);
    m_chainPositions.push_back(enemy->gridPos());  // 记录第一个命中敌人位置
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
        m_chainPositions.push_back(nextTarget->gridPos());  // 记录连锁敌人位置
        nextTarget->takeDamage(m_damage);
        origin = nextTarget->gridPos();
        --m_chainCount;
    }
}

void LightningBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;

    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(QColor(100, 180, 255), 2.5, Qt::SolidLine, Qt::RoundCap);
    p.setPen(pen);

    // 绘制每段闪电链
    for (size_t i = 0; i + 1 < m_chainPositions.size(); ++i) {
        const QPointF& start = m_chainPositions[i];
        const QPointF& end = m_chainPositions[i + 1];

        QPointF startPos(offsetX + start.x() * cellSize,
                        offsetY + start.y() * cellSize);
        QPointF endPos(offsetX + end.x() * cellSize,
                      offsetY + end.y() * cellSize);

        QPointF mid((startPos.x() + endPos.x()) / 2.0, (startPos.y() + endPos.y()) / 2.0);

        int j1 = rand() % 16 - 8;
        int j2 = rand() % 16 - 8;

        QPointF p1(startPos.x() + (mid.x() - startPos.x()) / 3.0 + j1,
                   startPos.y() + (mid.y() - startPos.y()) / 3.0 + j2);
        QPointF p2(mid.x() + j2, mid.y() + j1);
        QPointF p3(mid.x() + (endPos.x() - mid.x()) / 3.0 * 2.0 + j1,
                   mid.y() + (endPos.y() - mid.y()) / 3.0 * 2.0 - j2);

        QPainterPath path;
        path.moveTo(startPos);
        path.lineTo(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        path.lineTo(endPos);
        p.drawPath(path);

        QPen glow(QColor(255, 255, 255, 100), 1.0, Qt::SolidLine, Qt::RoundCap);
        p.setPen(glow);
        p.drawPath(path);

        p.setPen(pen);
    }
}