#ifndef FREEZEMARKER_H
#define FREEZEMARKER_H

#include "marker.h"

class FreezeMarker : public Marker {
public:
    FreezeMarker(int stackThreshold, double freezeDuration, double duration);

    void update(double dt) override;
    bool isActive() const override;
    std::unique_ptr<Marker> clone() const override;
    QString type() const override { return "freeze"; }
    double priority() const override { return 1.0; }

    double speedFactor() const override;
    double poisonDps() const override { return 0; }

    int stackCount() const override { return m_stackCount; }
    int stackThreshold() const override { return m_stackThreshold; }
    void apply(int stacks) override { m_stackCount += stacks; }
    std::unique_ptr<Marker> createTriggered(int) const override {
        return std::make_unique<FreezeMarker>(0, m_freezeDuration, m_freezeDuration);
    }

private:
    int m_stackThreshold;
    double m_freezeDuration;
    double m_duration;
    int m_stackCount = 0;
};

#endif