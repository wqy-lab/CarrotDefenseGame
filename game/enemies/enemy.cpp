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

    updateMarkers(dt);

    if (m_pathIndex >= static_cast<int>(m_path.size())) { m_reachedEnd = true; return; }

    QPointF target = QPointF(m_path[m_pathIndex].x() + 0.5, m_path[m_pathIndex].y() + 0.5);
    QPointF dir = target - m_gridPos;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    double moveDist = speed() * dt;
    if (moveDist >= dist) { m_gridPos = target; ++m_pathIndex; }
    else { dir /= dist; m_gridPos += dir * moveDist; }
}

void Enemy::takeDamage(double dmg) { m_hp -= dmg; if (m_hp < 0) m_hp = 0; }

double Enemy::speed() const {
    double factor = 1.0;
    for (const auto& [type, markers] : m_markers) {
        for (const auto& m : markers) {
            if (m->isActive()) {
                factor = std::min(factor, m->speedFactor());
            }
        }
    }
    return m_stats.speed * factor;
}

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
    QPointF pixelPos = pos(cellSize, offsetX, offsetY);
    double cx = pixelPos.x(), cy = pixelPos.y();

    double scale = cellSize * 0.75;
    if (m_textureTag == "boss")  scale = cellSize * 0.90;
    if (m_textureTag == "swarm") scale = cellSize * 0.50;
    if (m_textureTag == "tank")  scale = cellSize * 0.80;
    double half = scale / 2.0;

    int barW = static_cast<int>(scale);
    int barH = 4;
    p.fillRect(QRectF(cx - barW / 2.0, cy - half - 6, barW, barH), QColor(40, 40, 40));
    double hpR = m_hp / maxHp();
    QColor hpC = hpR > 0.5 ? QColor(76,175,80) : hpR > 0.25 ? QColor(255,193,7) : QColor(244,67,54);
    p.fillRect(QRectF(cx - barW / 2.0, cy - half - 6, barW * hpR, barH), hpC);

    QRectF targetRect(cx - half, cy - half, scale, scale);
    bool hasState = false;
    double slowFactor = 1.0;
    double poisonDps = 0.0;
    for (const auto& [type, markers] : m_markers) {
        for (const auto& m : markers) {
            if (m->isActive()) {
                if (m->speedFactor() < 1.0) hasState = true;
                if (m->poisonDps() > 0) hasState = true;
                slowFactor = std::min(slowFactor, m->speedFactor());
                poisonDps = std::max(poisonDps, m->poisonDps());
            }
        }
    }

    if (!m_textureTag.isEmpty()) {
        QString path = QString("assets/enemies/enemy_%1.png").arg(m_textureTag);
        const QPixmap& tex = DataManager::instance().getTexture(path);
        if (!tex.isNull()) {
            if (hasState) {
                QPixmap tinted = tex.scaled(static_cast<int>(scale), static_cast<int>(scale),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);
                QPainter tp(&tinted);
                tp.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                if (slowFactor < 1.0)
                    tp.fillRect(tinted.rect(), QColor(100, 180, 255, 80));
                if (poisonDps > 0)
                    tp.fillRect(tinted.rect(), QColor(120, 200, 80, 80));
                tp.end();
                p.drawPixmap(targetRect.toRect(), tinted);
            } else {
                p.drawPixmap(targetRect.toRect(), tex);
            }
        }
    }
}

// ==================== Marker Management ====================

void Enemy::addMarker(std::unique_ptr<Marker> marker) {
    QString type = marker->type();
    auto& markers = m_markers[type];

    // 累加层数
    marker->apply(1);
    markers.push_back(std::move(marker));

    // 检查是否触发
    if (markers.empty()) return;
    auto& first = markers.front();
    if (first->stackThreshold() > 0) {
        int total = 0;
        for (const auto& m : markers) {
            total += m->stackCount();
        }
        if (total >= first->stackThreshold()) {
            // 触发：清空 vector，插入触发后的 marker（由 Marker 自己创建）
            auto triggered = first->createTriggered(total);
            markers.clear();
            markers.push_back(std::move(triggered));
        }
    }
}

void Enemy::updateMarkers(double dt) {
    double poisonDps = 0.0;
    for (auto& [type, markers] : m_markers) {
        for (auto& m : markers) {
            m->update(dt);
            if (m->isActive()) {
                poisonDps = std::max(poisonDps, m->poisonDps());
            }
        }
        // 移除 inactive markers
        markers.erase(
            std::remove_if(markers.begin(), markers.end(),
                [](const std::unique_ptr<Marker>& m) { return !m->isActive(); }),
            markers.end()
        );
    }
    if (poisonDps > 0) {
        m_hp -= poisonDps * dt;
        if (m_hp < 0) m_hp = 0;
    }
}

void Enemy::removeInactiveMarkers() {
    for (auto& [type, markers] : m_markers) {
        markers.erase(
            std::remove_if(markers.begin(), markers.end(),
                [](const std::unique_ptr<Marker>& m) { return !m->isActive(); }),
            markers.end()
        );
    }
}