#ifndef POISONMARKER_H
#define POISONMARKER_H

#include "marker.h"

class PoisonMarker : public Marker {
public:
    PoisonMarker(double dps, double duration);

    void update(double dt) override;
    bool isActive() const override;
    std::unique_ptr<Marker> clone() const override;
    QString type() const override { return "poison"; }
    double priority() const override { return m_dps; }

    double speedFactor() const override { return 1.0; }
    double poisonDps() const override;

    int stackCount() const override { return 0; }
    int stackThreshold() const override { return 0; }
    void apply(int) override {}
    std::unique_ptr<Marker> createTriggered(int) const override { return clone(); }

private:
    double m_dps;
    double m_duration;
};

#endif