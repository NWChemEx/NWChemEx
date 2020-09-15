#include "nwchemex/load_modules.hpp"
#include <integrals/integralsmm.hpp>
#include <mp2/mp2_mm.hpp>
#include <scf/scf_mm.hpp>

namespace {

void set_scf_default_modules(sde::ModuleManager& mm) {
    mm.change_submod("CoreH", "T Builder", "Kinetic");
    mm.change_submod("CoreH", "V Builder", "Nuclear");
    mm.change_submod("DFJK", "3CERI Builder", "ERI3");
    mm.change_submod("DFJ", "3CERI Builder", "ERI3");
    mm.change_submod("CanJK", "ERI Builder", "ERI4");
    mm.change_submod("CanJ", "ERI Builder", "ERI4");
    mm.change_submod("MetricChol", "M Builder", "ERI2");
    mm.change_submod("DiagonalUpdate", "S Builder", "Overlap");
}

void set_mp2_default_modules(sde::ModuleManager& mm) {
    mm.change_submod("MP2", "ERI Builder", "ERI4");
    mm.change_submod("DOI SparseMap Builder", "dois", "DOI");
    mm.change_submod("LMO 2 AO", "dois", "DOI");
    mm.change_submod("DLPNO", "Fock Builder", "Fock");
    mm.change_submod("Dipole Center", "Dipole Builder", "EDipole");
    mm.change_submod("Transistion Dipole", "Dipole Builder", "EDipole");
    mm.change_submod("MP2 3-Center Sparse K", "DF Builder", "MetricChol");
    mm.change_submod("MP2 3-Center Sparse K", "ERI Builder", "ERI3");
    mm.change_submod("MP2 4-Center Sparse K", "ERI Builder", "ERI4");
}

} // namespace


namespace nwx {

void load_modules(sde::ModuleManager& mm) {

    integrals::load_modules(mm);
    scf::load_modules(mm);
    mp2::load_modules(mm);

    set_scf_default_modules(mm);
    set_mp2_default_modules(mm);
}

} // namespace nwx

