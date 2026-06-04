#ifndef SLOWMARKER_H
#define SLOWMARKER_H

#include "marker.h"

class SlowMarker : public Marker {
public:
    SlowMarker(double factor, double duration);

    void update(double dt, Enemy* enemy) override;
    bool isActive() const override;
    std::unique_ptr<Marker> clone() const override;
    QString type() const override { return "slow"; }
    double priority() const override { return 1.0 / m_factor; }  // 小factor=强=大priority

    double slowFactor() const { return m_factor; }
    double duration() const { return m_duration; }

private:
    double m_factor;      // 减速系数，越小越慢
    double m_duration;    // 剩余持续时间
};

#endif