#include "freezemarker.h"
#include "../enemies/enemy.h"

FreezeMarker::FreezeMarker(int stackThreshold, double freezeDuration, double duration)
    : m_stackThreshold(stackThreshold)
    , m_freezeDuration(freezeDuration)
    , m_duration(duration)
{
}

void FreezeMarker::update(double dt) {
    if (isActive()) {
        m_duration -= dt;
        if (m_duration <= 0) m_duration = 0;
    }
}

bool FreezeMarker::isActive() const {
    if (m_stackThreshold == 0) {
        // 触发后的状态，看 duration
        return m_duration > 0;
    }
    // 堆叠中，只要还有 stack 就视为活跃（用于后续命中检测）
    return m_stackCount > 0;
}

std::unique_ptr<Marker> FreezeMarker::clone() const {
    return std::make_unique<FreezeMarker>(m_stackThreshold, m_freezeDuration, m_duration);
}

double FreezeMarker::speedFactor() const {
    // 只有触发后（stackThreshold=0）且激活时才冰冻
    if (m_stackThreshold == 0 && isActive()) {
        return 0.0;
    }
    return 1.0;
}