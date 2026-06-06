#include "markerfactory.h"
#include "slowmarker.h"
#include "poisonmarker.h"
#include "freezemarker.h"

MarkerFactory& MarkerFactory::instance() {
    static MarkerFactory inst;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        inst.registerMarker("slow", [](const MarkerConfig& cfg) {
            return std::make_unique<SlowMarker>(cfg.factor, cfg.duration);
        });
        inst.registerMarker("poison", [](const MarkerConfig& cfg) {
            return std::make_unique<PoisonMarker>(cfg.factor, cfg.duration);
        });
        inst.registerMarker("freeze", [](const MarkerConfig& cfg) {
            return std::make_unique<FreezeMarker>(cfg.stackThreshold, cfg.freezeDuration, cfg.duration);
        });
    }
    return inst;
}

void MarkerFactory::registerMarker(const QString& type, Creator creator) {
    m_creators[type] = std::move(creator);
}

std::unique_ptr<Marker> MarkerFactory::create(const MarkerConfig& cfg) const {
    auto it = m_creators.find(cfg.type);
    if (it != m_creators.end()) {
        return it->second(cfg);
    }
    return nullptr;
}