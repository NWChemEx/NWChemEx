#pragma once
#include <pluginplay/pluginplay.hpp>

namespace nwx {

/** @brief Convenience function for loading all of NWChemEx's modules into an
 *         existing ModuleManager instance.
 *
 *  @param[in] mm The ModuleManager instance to load the modules into.
 *
 *  @throw std::bad_alloc if there is insufficient memory to create the new
 *                        modules. Weak throw guarantee.
 */
void load_modules(pluginplay::ModuleManager& mm);

} // namespace nwx
