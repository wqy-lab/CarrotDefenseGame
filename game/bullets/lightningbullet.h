#ifndef LIGHTNINGBULLET_H
#define LIGHTNINGBULLET_H

#include "bullet.h"
#include <set>
#include <vector>

class LightningBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;
    bool shouldDeactivate() const override { return false; }

public:
    LightningBullet(const QPointF& start, const QPointF& direction, double damage,
                    int chainCount, const QColor& color);

    void update(double dt, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;
    void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const override;

private:
    int m_chainCount;
    std::set<Enemy*> m_chainedEnemies;
    std::vector<QPointF> m_chainPositions;  // 存储所有连锁位置的网格坐标
    double m_lifetime = 0.3;  // 存活时间
    double m_lifetimeLeft = 0.3;  // 剩余存活时间
};

#endif