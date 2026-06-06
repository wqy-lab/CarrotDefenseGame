#ifndef SLOWMARKER_H
#define SLOWMARKER_H

#include "marker.h"

class SlowMarker : public Marker {
public:
    SlowMarker(double factor, double duration);

    void update(double dt) override;
    bool isActive() const override;
    std::unique_ptr<Marker> clone() const override;
    QString type() const override { return "slow"; }
    double priority() const override { return 1.0 / m_factor; }

    double speedFactor() const override;
    double poisonDps() const override { return 0; }

    int stackCount() const override { return 0; }
    int stackThreshold() const override { return 0; }
    void apply(int) override {}
    std::unique_ptr<Marker> createTriggered(int) const override { return clone(); }

private:
    double m_factor;
    double m_duration;
};

#endif