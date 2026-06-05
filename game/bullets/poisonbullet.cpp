#include "poisonbullet.h"
#include "../enemies/enemy.h"
#include "../config/datamanager.h"
#include <QtMath>

PoisonBullet::PoisonBullet(const QPointF& start, const QPointF& target, double damage, const QColor& color)
    : Bullet(start, target, damage, color)
{
}

void PoisonBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    for (auto& m : m_markerTemplates) {
        enemy->addMarker(m->clone());
    }
}

void PoisonBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;

    const QPixmap& tex = DataManager::instance().getTexture(
        "assets/bullets/poison_projectile.png");

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