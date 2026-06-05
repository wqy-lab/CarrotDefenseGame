#ifndef LIGHTNINGBULLET_H
#define LIGHTNINGBULLET_H

#include "bullet.h"
#include <set>

class LightningBullet : public Bullet {
protected:
    void onHit(Enemy* enemy, std::vector<std::unique_ptr<Enemy>>& enemies, CellEntities& cell) override;

public:
    LightningBullet(const QPointF& start, const QPointF& target, double damage,
                    int chainCount, const QColor& color);

    void draw(QPainter& p, double cellSize, double offsetX, double offsetY) const override;

private:
    int m_chainCount;
    std::set<Enemy*> m_chainedEnemies;
};

#endif