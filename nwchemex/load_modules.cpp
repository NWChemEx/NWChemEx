#include "nwchemex/load_modules.hpp"
#include <integrals/integralsmm.hpp>
#include <mp2/mp2_mm.hpp>
#include <scf/scf_mm.hpp>
#include <property_types/ao_integrals/kinetic.hpp>
#include <property_types/ao_integrals/nuclear.hpp>

namespace {

// Sets the project-wide defaults for modules in the SCF repo
void scf_defaults(sde::ModuleManager& mm) {

}

}

namespace nwx {

void load_modules(sde::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);
    mp2::load_modules(mm);
}

} // namespace nwx

