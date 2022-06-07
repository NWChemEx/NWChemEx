#pragma once
#include <pluginplay/pluginplay.hpp>

namespace nwchemex {

DECLARE_MODULE(ReferenceEnergyDriver);
DECLARE_MODULE(CorrelationEnergyDriver);

namespace drivers {

inline void load_modules(pluginplay::ModuleManager& mm) {
    mm.add_module<ReferenceEnergyDriver>("SCF Energy");
    mm.add_module<CorrelationEnergyDriver>("MP2 Energy");
}

} // namespace drivers

} // namespace nwchemex
