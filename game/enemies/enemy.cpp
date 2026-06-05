#include "enemy.h"
#include "../config/datamanager.h"
#include <QtMath>
#include <algorithm>

Enemy::Enemy(const std::vector<QPointF>& path, EnemyStats stats,
             const QString& textureTag)
    : m_stats(stats)
    , m_hp(stats.maxHp)
    , m_gridPos(path.empty() ? QPointF(0.5, 0.5) : QPointF(path[0].x() + 0.5, path[0].y() + 0.5))
    , m_path(path), m_pathIndex(1)
    , m_reachedEnd(false)
    , m_slowFactor(1.0), m_slowTimer(0.0)
    , m_poisonDps(0.0), m_poisonTimer(0.0)
    , m_goldAwarded(false)
    , m_textureTag(textureTag)
{}

bool Enemy::consumeReward()
{
    if (m_goldAwarded) return false;
    m_goldAwarded = true;
    return true;
}

void Enemy::update(double dt)
{
    if (m_reachedEnd || isDead()) return;

    if (m_slowTimer > 0) {
        m_slowTimer -= dt;
        if (m_slowTimer <= 0) { m_slowFactor = 1.0; m_slowTimer = 0; }
    }
    if (m_poisonTimer > 0) {
        m_hp -= m_poisonDps * dt;
        m_poisonTimer -= dt;
        if (m_poisonTimer <= 0) { m_poisonDps = 0; m_poisonTimer = 0; }
    }

    updateMarkers(dt);

    if (m_pathIndex >= static_cast<int>(m_path.size())) { m_reachedEnd = true; return; }

    QPointF target = QPointF(m_path[m_pathIndex].x() + 0.5, m_path[m_pathIndex].y() + 0.5);
    QPointF dir = target - m_gridPos;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    double moveDist = speed() * dt;
    if (moveDist >= dist) { m_gridPos = target; ++m_pathIndex; }
    else { dir /= dist; m_gridPos += dir * moveDist; }
}

void Enemy::applySlow(double factor, double duration) {
    if (factor < m_slowFactor) { m_slowFactor = factor; m_slowTimer = duration; }
}

void Enemy::applyPoison(double dps, double duration) {
    if (dps > m_poisonDps) { m_poisonDps = dps; m_poisonTimer = duration; }
}

void Enemy::takeDamage(double dmg) { m_hp -= dmg; if (m_hp < 0) m_hp = 0; }

void Enemy::updatePath(const std::vector<QPointF>& newPath)
{
    m_path = newPath;
    m_pathIndex = 0;
    double bestDist = 1e9;
    for (size_t i = 0; i < newPath.size(); ++i) {
        QPointF pathPoint(newPath[i].x() + 0.5, newPath[i].y() + 0.5);
        QPointF d = pathPoint - m_gridPos;
        double dist = d.x()*d.x() + d.y()*d.y();
        if (dist < bestDist) { bestDist = dist; m_pathIndex = static_cast<int>(i) + 1; }
    }
    if (m_pathIndex >= static_cast<int>(m_path.size())) m_reachedEnd = true;
}

QPointF Enemy::pos(double cellSize, double offsetX, double offsetY) const
{
    return QPointF(
        offsetX + m_gridPos.x() * cellSize,
        offsetY + m_gridPos.y() * cellSize);
}

void Enemy::draw(QPainter& p, double cellSize, double offsetX, double offsetY) const
{
    if (!isActive()) return;
    int r = static_cast<int>(radius());
    QPointF pixelPos = pos(cellSize, offsetX, offsetY);
    double cx = pixelPos.x(), cy = pixelPos.y();

    int barW = r * 3;
    int barH = 4;
    p.fillRect(QRectF(cx - barW/2.0, cy - r - 10, barW, barH), QColor(40,40,40));
    double hpR = m_hp / maxHp();
    QColor hpC = hpR > 0.5 ? QColor(76,175,80) : hpR > 0.25 ? QColor(255,193,7) : QColor(244,67,54);
    p.fillRect(QRectF(cx - barW/2.0, cy - r - 10, barW * hpR, barH), hpC);

    bool drewTexture = false;
    if (!m_textureTag.isEmpty()) {
        QString path = QString("assets/enemies/enemy_%1.png").arg(m_textureTag);
        const QPixmap& tex = DataManager::instance().getTexture(path);
        if (!tex.isNull()) {
            QRectF target(cx - r, cy - r, r * 2, r * 2);
            p.drawPixmap(target.toRect(), tex);
            drewTexture = true;
        }
    }

    if (!drewTexture) {
        QColor body = color();
        p.setPen(Qt::NoPen);
        p.setBrush(body);
        drawBody(p, QPointF(cx, cy), r);
    }

    QRectF stateRect(cx - r, cy - r, r * 2, r * 2);
    if (m_slowFactor < 1.0) {
        p.fillRect(stateRect, QColor(100, 180, 255, 80));
    }
    if (m_poisonTimer > 0) {
        p.fillRect(stateRect, QColor(120, 200, 80, 80));
    }
}

void Enemy::drawFace(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(center.x() - r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.drawEllipse(QPointF(center.x() + r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.setBrush(Qt::black);
    p.drawEllipse(QPointF(center.x() - r*0.25, center.y() - r*0.27), r*0.11, r*0.11);
    p.drawEllipse(QPointF(center.x() + r*0.35, center.y() - r*0.27), r*0.11, r*0.11);
}

// ==================== Marker Management ====================

void Enemy::addMarker(std::unique_ptr<Marker> marker) {
    QString type = marker->type();
    double incomingPriority = marker->priority();

    auto& slot = m_markers[type];

    if (!slot.active) {
        // No active marker, activate this one
        slot.active = std::move(marker);
        return;
    }

    double activePriority = slot.active->priority();

    if (incomingPriority > activePriority) {
        // Incoming is stronger - move active to pending, activate incoming
        slot.pending.push_back(std::move(slot.active));
        // Sort pending by priority descending
        std::sort(slot.pending.begin(), slot.pending.end(),
            [](const std::unique_ptr<Marker>& a, const std::unique_ptr<Marker>& b) {
                return a->priority() > b->priority();
            });
        slot.active = std::move(marker);
    } else {
        // Incoming is weaker or equal - add to pending
        slot.pending.push_back(std::move(marker));
        std::sort(slot.pending.begin(), slot.pending.end(),
            [](const std::unique_ptr<Marker>& a, const std::unique_ptr<Marker>& b) {
                return a->priority() > b->priority();
            });
    }
}

void Enemy::updateMarkers(double dt) {
    for (auto& pair : m_markers) {
        auto& slot = pair.second;
        if (slot.active) {
            slot.active->update(dt, this);
            if (!slot.active->isActive()) {
                slot.active.reset();
                // Promote next pending
                if (!slot.pending.empty()) {
                    slot.active = std::move(slot.pending.front());
                    slot.pending.erase(slot.pending.begin());
                }
            }
        }
    }
}

void Enemy::removeInactiveMarkers() {
    for (auto& pair : m_markers) {
        auto& slot = pair.second;
        if (slot.active && !slot.active->isActive()) {
            slot.active.reset();
            if (!slot.pending.empty()) {
                slot.active = std::move(slot.pending.front());
                slot.pending.erase(slot.pending.begin());
            }
        }
    }
}