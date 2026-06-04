#ifndef POISONMARKER_H
#define POISONMARKER_H

#include "marker.h"

class PoisonMarker : public Marker {
public:
    PoisonMarker(double dps, double duration);

    void update(double dt, Enemy* enemy) override;
    bool isActive() const override;
    std::unique_ptr<Marker> clone() const override;
    QString type() const override { return "poison"; }
    double priority() const override { return m_dps; }

    double poisonDps() const { return m_dps; }
    double duration() const { return m_duration; }

private:
    double m_dps;         // 每秒中毒伤害
    double m_duration;    // 剩余持续时间
};

#endif