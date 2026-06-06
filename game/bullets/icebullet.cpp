#include "icebullet.h"
#include "../enemies/enemy.h"
#include "../config/datamanager.h"
#include <QtMath>

IceBullet::IceBullet(const QPointF& start, const QPointF& direction, double damage, const QColor& color)
    : Bullet(start, direction, damage, color)
{
}

void IceBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);
    for (auto& m : m_markerTemplates) {
        enemy->addMarker(m->clone());
    }
}

void IceBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;

    const QPixmap& tex = DataManager::instance().getTexture(
        "assets/bullets/ice_projectile.png");

    QPointF pixelPos(offsetX + m_pos.x() * cellSize,
                     offsetY + m_pos.y() * cellSize);

    if (!tex.isNull()) {
        p.save();
        p.translate(pixelPos);
        double angle = qAtan2(m_direction.y(), m_direction.x());
        p.rotate(qRadiansToDegrees(angle));
        double sz = cellSize * 0.8;
        p.drawPixmap(QRectF(-sz / 2.0, -sz / 2.0, sz, sz).toRect(), tex);
        p.restore();
        return;
    }

    Bullet::draw(p, cellSize, offsetX, offsetY);
}