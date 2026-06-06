#include "slowmarker.h"
#include "../enemies/enemy.h"

SlowMarker::SlowMarker(double factor, double duration)
    : m_factor(factor)
    , m_duration(duration)
{
}

void SlowMarker::update(double dt) {
    if (isActive()) {
        m_duration -= dt;
        if (m_duration <= 0) m_duration = 0;
    }
}

bool SlowMarker::isActive() const {
    return m_duration > 0;
}

std::unique_ptr<Marker> SlowMarker::clone() const {
    return std::make_unique<SlowMarker>(m_factor, m_duration);
}

double SlowMarker::speedFactor() const {
    return isActive() ? m_factor : 1.0;
}