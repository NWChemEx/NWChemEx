#include "nwchemex/load_modules.hpp"
#include <integrals/integralsmm.hpp>
#include <scf/scf_mm.hpp>

namespace nwx {

void load_modules(sde::ModuleManager& mm) {
    //SCF right now includes this, uncomment when fixed
    //integrals::libint::load_modules(mm);
    scf::load_modules(mm);
}

} // namespace nwx

