#include "arrowbullet.h"
#include "../enemies/enemy.h"
#include "../markers/slowmarker.h"
#include "../markers/poisonmarker.h"
#include "../config/datamanager.h"
#include <QtMath>

ArrowBullet::ArrowBullet(const QPointF& start, const QPointF& target, double damage,
                         double slowFactor, double slowDuration,
                         double poisonDps, double poisonDuration, const QColor& color)
    : Bullet(start, target, damage, color)
{
    if (slowFactor < 1.0 && slowDuration > 0) {
        m_markers.push_back(std::make_unique<SlowMarker>(slowFactor, slowDuration));
    }
    if (poisonDps > 0 && poisonDuration > 0) {
        m_markers.push_back(std::make_unique<PoisonMarker>(poisonDps, poisonDuration));
    }
}

void ArrowBullet::onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell)
{
    Q_UNUSED(enemies);
    Q_UNUSED(cell);
    enemy->takeDamage(m_damage);
    for (auto& marker : m_markers) {
        enemy->addMarker(marker->clone());
    }
}

void ArrowBullet::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!m_active) return;

    const QPixmap& tex = DataManager::instance().getTexture(
        "assets/bullets/arrow_projectile.png");

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