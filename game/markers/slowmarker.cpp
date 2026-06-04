#include "slowmarker.h"
#include "../enemies/enemy.h"

SlowMarker::SlowMarker(double factor, double duration)
    : m_factor(factor)
    , m_duration(duration)
{
}

void SlowMarker::update(double dt, Enemy* enemy) {
    if (!enemy || !isActive()) return;
    enemy->applySlow(m_factor, m_duration);
    m_duration -= dt;
}

bool SlowMarker::isActive() const {
    return m_duration > 0;
}

std::unique_ptr<Marker> SlowMarker::clone() const {
    return std::make_unique<SlowMarker>(m_factor, m_duration);
}