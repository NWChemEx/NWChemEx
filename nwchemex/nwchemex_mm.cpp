#include "nwchemex/nwchemex_mm.hpp"
#include <integrals/integralsmm.hpp>
#include <scf/scf_mm.hpp>

namespace nwchemex {

void load_modules(sde::ModuleManager& mm) {
    //uncomment when SCF doesn't call it
    //integrals::load_mm(mm);
    scf::load_modules(mm);
}

}
