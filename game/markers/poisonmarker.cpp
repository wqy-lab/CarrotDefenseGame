#include "poisonmarker.h"
#include "../enemies/enemy.h"

PoisonMarker::PoisonMarker(double dps, double duration)
    : m_dps(dps)
    , m_duration(duration)
{
}

void PoisonMarker::update(double dt) {
    if (isActive()) {
        m_duration -= dt;
        if (m_duration <= 0) m_duration = 0;
    }
}

bool PoisonMarker::isActive() const {
    return m_duration > 0;
}

std::unique_ptr<Marker> PoisonMarker::clone() const {
    return std::make_unique<PoisonMarker>(m_dps, m_duration);
}

double PoisonMarker::poisonDps() const {
    return isActive() ? m_dps : 0.0;
}