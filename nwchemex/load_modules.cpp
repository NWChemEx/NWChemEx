#include "nwchemex/load_modules.hpp"
#include <integrals/integralsmm.hpp>
#include <scf/scf_mm.hpp>
#include <property_types/ao_integrals/kinetic.hpp>
#include <property_types/ao_integrals/nuclear.hpp>

namespace nwx {

void load_modules(sde::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);

    mm.change_submod("CoreH", "T Builder", "Kinetic");
    mm.change_submod("CoreH", "V Builder", "Nuclear");
    mm.change_submod("DFJK", "3CERI Builder", "ERI3");
    mm.change_submod("DFJ", "3CERI Builder", "ERI3");
    mm.change_submod("CanJK", "ERI Builder", "ERI4");
    mm.change_submod("CanJ", "ERI Builder", "ERI4");
    mm.change_submod("MetricChol", "M Builder", "ERI2");
    mm.change_submod("DiagonalUpdate", "S Builder", "Overlap");
}

} // namespace nwx

