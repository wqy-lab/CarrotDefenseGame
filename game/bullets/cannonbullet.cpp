#include "cannonbullet.h"
#include "../enemies/enemy.h"
#include "../config/datamanager.h"
#include <QtMath>

CannonBullet::CannonBullet(const QPointF& start, const QPointF& target, double damage,
                            double splashRadius, const QColor& color)
    : Bullet(start, target, damage, color)
    , m_splashRadius(splashRadius)
{
}

void CannonBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);

    if (m_splashRadius > 0) {
        for (auto& e : enemies) {
            if (!e->isActive() || e.get() == enemy) continue;
            QPointF d = e->gridPos() - m_pos;
            double dist2 = d.x()*d.x() + d.y()*d.y();
            if (dist2 <= m_splashRadius * m_splashRadius) {
                e->takeDamage(m_damage * 0.5);
            }
        }
    }
}

void CannonBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;

    const QPixmap& tex = DataManager::instance().getTexture(
        "assets/bullets/cannon_projectile.png");

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