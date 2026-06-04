#include "enemy.h"
#include <QtMath>
#include <algorithm>

Enemy::Enemy(const std::vector<QPointF>& path, EnemyStats stats)
    : m_stats(stats)
    , m_hp(stats.maxHp)
    , m_gridPos(path.empty() ? QPointF(0,0) : path[0])
    , m_path(path), m_pathIndex(1)
    , m_reachedEnd(false)
    , m_slowFactor(1.0), m_slowTimer(0.0)
    , m_poisonDps(0.0), m_poisonTimer(0.0)
    , m_goldAwarded(false)
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

    if (m_pathIndex >= static_cast<int>(m_path.size())) { m_reachedEnd = true; return; }

    QPointF target = m_path[m_pathIndex];
    QPointF dir = target - m_gridPos;
    double dist = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    // speed() is already in grids/sec (pixel speed / 48 reference cellSize)
    double moveDist = speed() * dt;
    if (moveDist >= dist) { m_gridPos = target; ++m_pathIndex; }
    else { dir /= dist; m_gridPos += dir * moveDist; }
}

void Enemy::applySlow(double factor, double duration) {
    if (factor < m_slowFactor) { m_slowFactor = factor; m_slowTimer = duration; }
}

void Enemy::applyPoison(double dps, double duration) {
    m_poisonDps = dps; m_poisonTimer = duration;
}

void Enemy::takeDamage(double dmg) { m_hp -= dmg; if (m_hp < 0) m_hp = 0; }

void Enemy::updatePath(const std::vector<QPointF>& newPath)
{
    m_path = newPath;
    m_pathIndex = 0;
    double bestDist = 1e9;
    for (size_t i = 0; i < newPath.size(); ++i) {
        QPointF d = newPath[i] - m_gridPos;
        double dist = d.x()*d.x() + d.y()*d.y();
        if (dist < bestDist) { bestDist = dist; m_pathIndex = static_cast<int>(i) + 1; }
    }
    if (m_pathIndex >= static_cast<int>(m_path.size())) m_reachedEnd = true;
}

QPointF Enemy::pos(double cellSize, double offsetX, double offsetY) const
{
    return QPointF(
        offsetX + m_gridPos.x() * cellSize + cellSize / 2.0,
        offsetY + m_gridPos.y() * cellSize + cellSize / 2.0);
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

    QColor body = color();
    if (m_poisonTimer > 0) body = QColor(120, 200, 80);
    else if (m_slowFactor < 1.0) body = QColor(100, 180, 255);

    p.setPen(Qt::NoPen);
    drawBody(p, QPointF(cx, cy), r);
}

void Enemy::drawFace(QPainter& p, const QPointF& center, int r) const {
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(center.x() - r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.drawEllipse(QPointF(center.x() + r*0.3, center.y() - r*0.25), r*0.22, r*0.22);
    p.setBrush(Qt::black);
    p.drawEllipse(QPointF(center.x() - r*0.25, center.y() - r*0.27), r*0.11, r*0.11);
    p.drawEllipse(QPointF(center.x() + r*0.35, center.y() - r*0.27), r*0.11, r*0.11);
}

