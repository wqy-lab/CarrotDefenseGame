#include "icebullet.h"
#include "../enemies/enemy.h"
#include "../markers/slowmarker.h"
#include "../config/datamanager.h"
#include <QtMath>

IceBullet::IceBullet(const QPointF& start, const QPointF& target, double damage,
                     double slowFactor, double slowDuration, const QColor& color)
    : Bullet(start, target, damage, color)
{
    if (slowFactor < 1.0 && slowDuration > 0) {
        m_marker = std::make_unique<SlowMarker>(slowFactor, slowDuration);
    }
}

void IceBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    if (m_marker) {
        enemy->addMarker(m_marker->clone());
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
        int hw = tex.width() / 2;
        int hh = tex.height() / 2;
        p.drawPixmap(-hw, -hh, tex);
        p.restore();
        return;
    }

    Bullet::draw(p, cellSize, offsetX, offsetY);
}