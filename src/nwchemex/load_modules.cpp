#include "modules.hpp"
#include "nwchemex/load_modules.hpp"
#include <integrals/integrals.hpp>
//#include <mp2/mp2_mm.hpp>
#include <scf/scf.hpp>

namespace {

void set_scf_default_modules(pluginplay::ModuleManager& mm) {
    mm.change_submod("CanJK", "ERI Builder", "ERI4");
    mm.change_submod("CanJ", "ERI Builder", "ERI4");
    mm.change_submod("CoreH", "Kinetic Energy", "Kinetic");
    mm.change_submod("CoreH", "Electron-Nuclear Attraction", "Nuclear");
    mm.change_submod("CoreGuess", "Overlap", "Overlap");
    mm.change_submod("SCF", "Overlap", "Overlap");
    // mm.change_submod("DFJ", "3CERI Builder", "ERI3");
    // mm.change_submod("DFJK", "3CERI Builder", "ERI3");
    // mm.change_submod("DiagonalUpdate", "S Builder", "Overlap");
    // mm.change_submod("MetricChol", "M Builder", "ERI2");
    // mm.change_submod("SCFDIIS", "S Builder", "Overlap");
}

// void set_mp2_default_modules(sde::ModuleManager& mm) {
//     mm.change_submod("DOI SparseMap Builder", "dois", "DOI");
//     mm.change_submod("LMO 2 AO", "dois", "DOI");
//     mm.change_submod("AO 2 Aux", "dois", "DOI");
//     mm.change_submod("DLPNO", "Fock Builder", "Fock");
//     mm.change_submod("Dipole Center", "Dipole Builder", "EDipole");
//     mm.change_submod("Transition Dipole", "Dipole Builder", "EDipole");
//     mm.change_submod("MP2 3-Center K", "(P | mu nu)", "ERI3");
//     mm.change_submod("MP2 3-Center K", "(Q | P)", "MetricChol");
//     mm.change_submod("MP2 3-Center Sparse K", "(Q | P)", "Sparse DF Coefs");
//     mm.change_submod("MP2 3-Center Sparse K", "(P | mu nu)", "ERI3");
//     mm.change_submod("MP2 4-Center K", "(mu nu | lambda sigma)", "ERI4");
//     mm.change_submod("MP2 4-Center Sparse K", "(mu nu | lambda sigma)",
//     "ERI4"); mm.change_submod("Sparse DF Coefs", "M Builder", "ERI2");
// }

} // namespace

namespace nwx {

void load_modules(pluginplay::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);
    // mp2::load_modules(mm);

    mm.add_module<SystemHamiltonian>("SystemHamiltonian");

    set_scf_default_modules(mm);
    // set_mp2_default_modules(mm);
}

} // namespace nwx
