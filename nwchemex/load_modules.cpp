#include "nwchemex/load_modules.hpp"
#include <integrals/integralsmm.hpp>
#include <scf/scf_mm.hpp>
#include <property_types/ao_integrals/kinetic.hpp>
#include <property_types/ao_integrals/nuclear.hpp>

namespace nwx {

void load_modules(sde::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);

    // TODO: move these defaults to the Integrals repo
    mm.set_default<property_types::KineticIntegral<double>>(std::string{"Kinetic"});
    mm.set_default<property_types::NuclearIntegral<double>>(std::string{"Nuclear"});
}

} // namespace nwx

