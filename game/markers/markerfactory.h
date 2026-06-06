#ifndef MARKERFACTORY_H
#define MARKERFACTORY_H

#include "marker.h"
#include "../config/datamanager.h"
#include <memory>
#include <functional>
#include <unordered_map>

class MarkerFactory {
public:
    using Creator = std::function<std::unique_ptr<Marker>(const MarkerConfig& cfg)>;

    static MarkerFactory& instance();

    void registerMarker(const QString& type, Creator creator);
    std::unique_ptr<Marker> create(const MarkerConfig& cfg) const;

private:
    MarkerFactory() = default;
    std::unordered_map<QString, Creator> m_creators;
};

#endif